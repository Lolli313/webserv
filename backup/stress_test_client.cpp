#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cerrno>
#include <mutex>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFERSIZE 4096
#define PORT "8080"

// Global atomic flags and counters
std::atomic<bool> stopRequested(false);
std::atomic<int> currentConnections(0);
std::atomic<int> successfulConnections(0);
std::atomic<int> failedConnections(0);
std::mutex printMutex;

// FIX 1: Signal handler must only use async-signal-safe operations.
// std::cout is NOT async-signal-safe. Just set the flag.
void handle_sigint(int) {
    stopRequested.store(true, std::memory_order_relaxed);
}

void raise_fd_limit(int newLimit) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
        std::cout << "Current RLIMIT_NOFILE: soft=" << rl.rlim_cur 
                  << " hard=" << rl.rlim_max << std::endl;
        
        rlim_t requested = static_cast<rlim_t>(newLimit);
        
        rl.rlim_cur = requested;
        
        if (rl.rlim_cur > rl.rlim_max) {
            rl.rlim_max = rl.rlim_cur;
        }
        
        if (setrlimit(RLIMIT_NOFILE, &rl) != 0) {
            std::cerr << "Warning: setrlimit(RLIMIT_NOFILE, " << newLimit 
                      << ") failed: " << std::strerror(errno) << std::endl;
            std::cerr << "Try running with sudo or adjusting /etc/security/limits.conf" << std::endl;
            
            if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
                rl.rlim_cur = rl.rlim_max;
                if (setrlimit(RLIMIT_NOFILE, &rl) == 0) {
                    std::cout << "Set RLIMIT_NOFILE to hard limit: " << rl.rlim_max << std::endl;
                }
            }
        } else {
            std::cout << "Successfully raised RLIMIT_NOFILE to " << newLimit << std::endl;
        }
        
        if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
            std::cout << "New RLIMIT_NOFILE: soft=" << rl.rlim_cur 
                      << " hard=" << rl.rlim_max << std::endl;
        }
    } else {
        std::cerr << "getrlimit(RLIMIT_NOFILE) failed: " << std::strerror(errno) << std::endl;
    }
}

void print_system_limits() {
    std::cout << "\n=== System Resource Limits ===" << std::endl;
    
    struct rlimit rl;
    
    if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
        std::cout << "RLIMIT_NOFILE (open files): soft=" << rl.rlim_cur 
                  << " hard=" << rl.rlim_max << std::endl;
    }
    
    if (getrlimit(RLIMIT_NPROC, &rl) == 0) {
        std::cout << "RLIMIT_NPROC (threads/processes): soft=" << rl.rlim_cur 
                  << " hard=" << rl.rlim_max << std::endl;
    }
    
    if (getrlimit(RLIMIT_STACK, &rl) == 0) {
        std::cout << "RLIMIT_STACK (thread stack): soft=" << rl.rlim_cur 
                  << " hard=" << rl.rlim_max << std::endl;
    }
    
    std::cout << "==============================\n" << std::endl;
}

void print_connection_status() {
    std::lock_guard<std::mutex> lock(printMutex);
    std::cout << "\rConnected clients: " << currentConnections.load() << std::flush;
}

void clientLoop(int clientId)
{
    int clientFD = -1;
    struct addrinfo prep, *res = nullptr;
    // FIX 2: Track whether we successfully connected so cleanup is correct
    bool connected = false;
    std::string readBuffer;
    readBuffer.resize(BUFFERSIZE);
    
    try {
        std::memset(&prep, 0, sizeof(addrinfo));
        prep.ai_family = AF_INET;
        prep.ai_socktype = SOCK_STREAM;
        
        int status = getaddrinfo("127.0.0.1", PORT, &prep, &res);
        
        if (status != 0) {
            failedConnections.fetch_add(1);
            return;
        }

        clientFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (clientFD == -1) {
            failedConnections.fetch_add(1);
            freeaddrinfo(res);
            res = nullptr;  // FIX 3: Null out after free to prevent double-free
            return;
        }

        if (connect(clientFD, res->ai_addr, res->ai_addrlen) == -1) {
            failedConnections.fetch_add(1);
            close(clientFD);
            clientFD = -1;  // FIX 3: Null out after close
            freeaddrinfo(res);
            res = nullptr;
            return;
        }

        // FIX 4: Free addrinfo immediately after connect — no longer needed
        freeaddrinfo(res);
        res = nullptr;

        // Successfully connected
        connected = true;
        successfulConnections.fetch_add(1);
        currentConnections.fetch_add(1);
        print_connection_status();

        // Send test request
        std::string testMessage = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
        ssize_t sent = send(clientFD, testMessage.c_str(), testMessage.size(), 0);
        
        if (sent <= 0) {
            currentConnections.fetch_sub(1);
            connected = false;
            print_connection_status();
            close(clientFD);
            clientFD = -1;
            return;
        }

        // Set socket timeout for recv
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms timeout
        setsockopt(clientFD, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        while (!stopRequested.load(std::memory_order_relaxed)) {
            ssize_t bytesRead = recv(clientFD, &readBuffer[0], readBuffer.size() - 1, 0);
            
            if (bytesRead > 0) {
                // Successfully received data
            } 
            else if (bytesRead == 0) {
                // Server closed connection
                break;
            }
            else {
                // FIX 5: Capture errno immediately — before any other call
                int savedErrno = errno;
                if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK) {
                    break;
                }
                // Timeout: loop back and check stopRequested
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Disconnecting
        currentConnections.fetch_sub(1);
        connected = false;
        print_connection_status();
        
        close(clientFD);
        clientFD = -1;
        
    } catch (const std::exception& e) {
        {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cerr << "\nClient " << clientId << " exception: " << e.what() << std::endl;
        }

        // FIX 6: Only decrement currentConnections if we actually connected
        if (connected) {
            currentConnections.fetch_sub(1);
            print_connection_status();
        }
        if (clientFD != -1) {
            close(clientFD);
        }
        if (res != nullptr) {
            freeaddrinfo(res);
        }
        failedConnections.fetch_add(1);
    }
}

int main(int argc, char* argv[])
{
    int numClients = 10;
    
    if (argc > 1) {
        numClients = std::atoi(argv[1]);
        if (numClients <= 0 || numClients > 100000) {
            std::cerr << "Invalid number of clients (must be 1-100000). Using default: 10" << std::endl;
            numClients = 10;
        }
    }
    
    std::cout << "=== Web Server Load Test Tool ===" << std::endl;
    std::cout << "Target: 127.0.0.1:" << PORT << std::endl;
    std::cout << "Number of clients: " << numClients << std::endl;
    
    print_system_limits();
    
    int requiredFDs = numClients + 100;
    raise_fd_limit(requiredFDs);
    
    // Install signal handlers
    std::signal(SIGINT, handle_sigint);
    std::signal(SIGTERM, handle_sigint);
    
    std::cout << "\nStarting " << numClients << " client threads..." << std::endl;
    std::cout << "Press Ctrl+C to stop all clients gracefully.\n" << std::endl;
    
    auto startTime = std::chrono::steady_clock::now();
    
    std::vector<std::thread> threads;
    threads.reserve(numClients);
    
    for (int i = 0; i < numClients; i++) {
        // FIX 7: Check stopRequested before launching more threads
        if (stopRequested.load(std::memory_order_relaxed)) {
            std::cout << "\nStop requested during thread creation at thread " << i << std::endl;
            break;
        }

        try {
            threads.emplace_back(clientLoop, i);
        } catch (const std::system_error& e) {
            std::cerr << "\nFailed to create thread " << i << ": " << e.what() 
                      << " (code: " << e.code() << ")" << std::endl;
            std::cerr << "Successfully created " << i << " threads before failure." << std::endl;
            break;
        }
    }
    
    int threadsCreated = static_cast<int>(threads.size());
    std::cout << "Successfully created " << threadsCreated << " client threads.\n" << std::endl;
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // FIX 1 (cont.): Print the interrupt message here instead of in the signal handler
    if (stopRequested.load()) {
        std::cout << "\nReceived interrupt signal. All clients stopped." << std::endl;
    }

    std::cout << "\n=== Test Results ===" << std::endl;
    std::cout << "Threads created: " << threadsCreated << std::endl;
    std::cout << "Successful connections: " << successfulConnections.load() << std::endl;
    std::cout << "Failed connections: " << failedConnections.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "====================" << std::endl;
    
    return 0;
}