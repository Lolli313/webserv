#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    // Usage:
    // ./slow_backpressure_client <ip> <port> <read_size> <initial_sleep_ms> <sleep_between_reads_ms>
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0]
                  << " <ip> <port> <read_size> <initial_sleep_ms> <sleep_between_reads_ms>\n";
        return 1;
    }

    std::string ip = argv[1];
    int port = std::stoi(argv[2]);
    int read_size = std::stoi(argv[3]);
    int initial_sleep_ms = std::stoi(argv[4]);
    int sleep_between_reads_ms = std::stoi(argv[5]);

    if (port <= 0 || port > 65535 || read_size <= 0 ||
        initial_sleep_ms < 0 || sleep_between_reads_ms < 0) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    // Make receive buffer tiny to increase pressure.
    int rcvbuf = 1024;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) < 0) {
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

    std::string req =
        "GET /imgs/pexels-souvenirpixels-417074.jpg HTTP/1.1\r\n"
        "Host: localhost:" + std::to_string(port) + "\r\n"
        "Connection: close\r\n\r\n";

    if (send(fd, req.c_str(), req.size(), MSG_NOSIGNAL) < 0) {
        std::cerr << "request send failed: " << std::strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    std::cout << "Request sent. Sleeping " << initial_sleep_ms
              << "ms before reading...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(initial_sleep_ms));

    std::vector<char> buf(static_cast<size_t>(read_size));
    size_t total = 0;

    while (true) {
        ssize_t n = recv(fd, buf.data(), buf.size(), 0);
        if (n > 0) {
            total += static_cast<size_t>(n);
            std::cout << "recv " << n << " bytes, total=" << total << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_between_reads_ms));
        } else if (n == 0) {
            std::cout << "server closed connection, total=" << total << "\n";
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