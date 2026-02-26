#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <csignal>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFERSIZE 4096
#define PORT "8080"

// Global atomic flag for graceful shutdown
std::atomic<bool> stopRequested(false);

void handle_sigint(int) {
    std::cout << "\nReceived interrupt signal. Stopping all clients..." << std::endl;
    stopRequested.store(true);
}

void clientLoop(int clientId)
{
    int clientFD = -1;
    struct addrinfo prep, *res;
    std::string readBuffer;
    readBuffer.resize(BUFFERSIZE);
    std::memset(&prep, 0, sizeof(addrinfo));

    prep.ai_family = AF_INET;
    prep.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo("127.0.0.1", PORT, &prep, &res);
    
    if (status != 0) {
        std::cerr << "Client " << clientId << ": getaddrinfo error: "
                  << gai_strerror(status) << std::endl;
        return;
    }

    clientFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (clientFD == -1) {
        std::cerr << "Client " << clientId << ": socket creation failed" << std::endl;
        freeaddrinfo(res);
        return;
    }

    std::cout << "Client " << clientId << " FD: " << clientFD << std::endl;

    if (connect(clientFD, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Client " << clientId << ": connect failed" << std::endl;
        close(clientFD);
        freeaddrinfo(res);
        return;
    }

    std::cout << "Client " << clientId << " connected successfully" << std::endl;

    // Send test request
    std::string testMessage = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
    ssize_t sent = send(clientFD, testMessage.c_str(), testMessage.size(), 0);
    
    if (sent <= 0) {
        std::cerr << "Client " << clientId << ": send failed" << std::endl;
        close(clientFD);
        freeaddrinfo(res);
        return;
    }

    // Receive loop - check stopRequested flag periodically
    while (!stopRequested.load()) {
        // Set socket to non-blocking or use timeout for recv
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms timeout
        setsockopt(clientFD, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        ssize_t bytesRead = recv(clientFD, &readBuffer[0], readBuffer.size() - 1, 0);
        
        if (bytesRead > 0) {
            readBuffer[bytesRead] = '\0';
            std::cout << "Client " << clientId << " received " << bytesRead << " bytes" << std::endl;
            // For testing, you might want to break after first response or keep reading
            // break; // Uncomment to exit after first response
        } 
        else if (bytesRead == 0) {
            // Server closed connection
            std::cout << "Client " << clientId << ": server closed connection" << std::endl;
            break;
        }
        else {
            // Timeout or error - check if we should stop
            if (stopRequested.load()) {
                break;
            }
            // Otherwise continue (timeout is normal for keep-alive)
        }
        
        // Optional: sleep briefly to avoid tight loop
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (stopRequested.load()) {
        std::cout << "Client " << clientId << ": stopping gracefully" << std::endl;
    }

    close(clientFD);
    freeaddrinfo(res);
}

int main(int argc, char* argv[])
{
    int numClients = 10;  // Default number of clients
    
    // Allow user to specify number of clients via command line
    if (argc > 1) {
        numClients = std::atoi(argv[1]);
        if (numClients <= 0 || numClients > 10000) {
            std::cerr << "Invalid number of clients. Using default: 10" << std::endl;
            numClients = 10;
        }
    }
    
    // Install signal handler for Ctrl+C
    std::signal(SIGINT, handle_sigint);
    std::signal(SIGTERM, handle_sigint);
    
    std::cout << "Starting test with " << numClients << " client threads..." << std::endl;
    std::cout << "Press Ctrl+C to stop all clients gracefully." << std::endl;
    
    std::vector<std::thread> threads;
    threads.reserve(numClients);
    
    // Launch all client threads
    for (int i = 0; i < numClients; i++) {
        threads.emplace_back(clientLoop, i);
        // Optional: add small delay between thread launches to avoid overwhelming server
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "All " << numClients << " client threads launched." << std::endl;
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    std::cout << "All client threads completed. Test finished." << std::endl;
    
    return 0;
}