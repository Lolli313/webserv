
#include "terminalColors.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <cstring>
#include <string>

#define BUFFERSIZE 1
#define MAX_EVENTS 5
#define TIMEOUT 30000

bool createEpollEvent(int epollFD, int targetFd, int epollEventFlag) {
	struct epoll_event event;

	event.events = epollEventFlag;
	event.data.fd = targetFd;	

	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, targetFd, &event)) {
		std::cerr << "Failed to create epoll event" << std::endl;
		close(epollFD);
		return (false);
	}
	return (true);
}

int main() {
	int serverFD, newSocket, readSize;
	struct addrinfo prep, *res;
	struct sockaddr_in address;
	std::memset(&prep, 0, sizeof(addrinfo));
	std::string readBuffer;
	std::string mainBuffer;
	int status = 0;
	
	int epollFD = -2;
	
	if ((epollFD = epoll_create1(0)) < 0) {
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		return (1);
	}

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	readBuffer.resize(BUFFERSIZE);
	mainBuffer.resize(BUFFERSIZE);

	if ((status = getaddrinfo("127.0.0.1", "8080", &prep, &res)) != 0) {
		std::cout << gai_strerror(status) << std::endl;
		std::cout << res->ai_flags << std::endl;
		freeaddrinfo(res);
		return (1);
	}

	if ((serverFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
		perror("socket");
		freeaddrinfo(res);
		return (1);
	}

	if (bind(serverFD, res->ai_addr, res->ai_addrlen) < 0) {
		perror("bind");
		freeaddrinfo(res);
		return (1);
	}

	if (listen(serverFD, 5) < 0) {
		perror("listen");
		freeaddrinfo(res);
		return (1);
	}

	if ((newSocket = accept(serverFD, res->ai_addr, &res->ai_addrlen)) < 0) { perror("accept"); freeaddrinfo(res); return (1); }
	
	if (!createEpollEvent(epollFD, newSocket, EPOLLIN))
	{
		std::cerr << RED << "epoll creation" << RESET << std::endl;
		return 1;
	}
	struct epoll_event eventArray[MAX_EVENTS];

	int eventCount;
	bool running = true;
	while (running)
	{
		eventCount = epoll_wait(epollFD, eventArray, MAX_EVENTS, TIMEOUT);
		std::cout << PURPLE << eventCount << " events ready" << RESET << std::endl;
		for (int i = 0; i < eventCount; i++) {
			readSize = -2;
			while ((readSize = read(newSocket, &readBuffer[0], readBuffer.size())) > 0) {
				if (readSize < 0) {
					perror("read");
					freeaddrinfo(res);
					return (1);
				}
				std::cout << "Received size = " << readSize << std::endl;
				mainBuffer.append(readBuffer.data(), readSize);
				// std::string responseString = "Hello back";
				// send(newSocket, responseString.c_str(), responseString.size(), 0);
			}
			if (mainBuffer.find("stop") == std::string::npos)
				running = false;
			std::cout << YELLOW << "Total size read: " << mainBuffer.size() << RESET << std::endl;
			std::cout << GREEN << "Read data from client:" << mainBuffer << RESET << std::endl;
			close(newSocket);
		}
	}
	if (close(epollFD)) {
		std::cerr << "Failed to close epoll file descriptor" << std::endl;
		return 1;
	}
	close(serverFD);
	freeaddrinfo(res);
	return (0);
}
