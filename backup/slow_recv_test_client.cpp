#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <thread>

int set_recvbuf(int fd, int size) {
    return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}

int main(int argc, char* argv[]) {
    // Usage:
    // ./slow_recv_test_client <ip> <port> <read_size> <sleep_ms_between_reads>
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0]
                  << " <ip> <port> <read_size> <sleep_ms_between_reads>\n";
        return 1;
    }

    std::string ip = argv[1];
    int port = std::stoi(argv[2]);
    int read_size = std::stoi(argv[3]);       // e.g. 1, 16, 64
    int sleep_ms = std::stoi(argv[4]);        // e.g. 100, 300, 1000

    if (port <= 0 || port > 65535 || read_size <= 0 || sleep_ms < 0) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    // Make receive buffer small to amplify backpressure.
    // Kernel may clamp/adjust, that's fine.
    int wanted_rcvbuf = 1024;
    if (set_recvbuf(fd, wanted_rcvbuf) < 0) {
        std::cerr << "setsockopt(SO_RCVBUF) failed: " << std::strerror(errno) << "\n";
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "invalid IP\n";
        close(fd);
        return 1;
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "connect failed: " << std::strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    std::cout << "Connected to " << ip << ":" << port
              << " read_size=" << read_size
              << " sleep_ms=" << sleep_ms << "\n";

    // Trigger a response from server (adjust path as needed).
    std::string req =
        "GET / HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n\r\n";
    ssize_t s = send(fd, req.c_str(), req.size(), MSG_NOSIGNAL);
    if (s < 0) {
        std::cerr << "send request failed: " << std::strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    std::vector<char> buf(static_cast<size_t>(read_size));
    size_t total = 0;
    int recv_calls = 0;

    while (true) {
        ssize_t n = recv(fd, buf.data(), buf.size(), 0);
        recv_calls++;

        if (n > 0) {
            total += static_cast<size_t>(n);
            std::cout << "[recv call " << recv_calls << "] got " << n
                      << " bytes (total=" << total << ")\n";

            // Intentionally slow down consumption
            if (sleep_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
            }
        } else if (n == 0) {
            std::cout << "Server closed connection. Total received=" << total
                      << ", recv calls=" << recv_calls << "\n";
            break;
        } else {
            if (errno == EINTR) continue;
            std::cerr << "recv failed: " << std::strerror(errno) << "\n";
            break;
        }
    }

    close(fd);
    return 0;
}