
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
#define TIMEOUT -1

bool createEpollEvent(int epollFD, int targetFd, int epollEvent, int epollEventFlag) {
	struct epoll_event event;

	event.events = epollEventFlag;
	event.data.fd = targetFd;	

	if (epoll_ctl(epollFD, epollEvent, targetFd, &event)) {
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

	// if ((newSocket = accept(serverFD, res->ai_addr, &res->ai_addrlen)) < 0) {
	// 	perror("accept");
	// 	freeaddrinfo(res);
	// 	return (1);
	// }
	
	if (!createEpollEvent(epollFD, serverFD, EPOLL_CTL_ADD, EPOLLIN)) {
		std::cerr << RED << "Failed to add serverFD to epoll pool" << RESET << std::endl;
		return 1;
	}
	struct epoll_event eventArray[MAX_EVENTS];

	int eventCount;
//	bool running = true;
	while (true) {
		eventCount = epoll_wait(epollFD, eventArray, MAX_EVENTS, TIMEOUT);
		std::cout << PURPLE << eventCount << " events ready" << RESET << std::endl;
		for (int i = 0; i < eventCount; i++) {
			int eventFD = eventArray[i].data.fd;
			if (eventFD == serverFD) { // a new client requests a connection
				std::cout << "Found a new connection" << std::endl;
				if ((newSocket = accept(eventFD, res->ai_addr, &res->ai_addrlen)) < 0) {
					freeaddrinfo(res);
					perror("accept");
					return (1);
				}
				createEpollEvent(epollFD, newSocket, EPOLL_CTL_ADD, EPOLLIN);
			}
			else { // old client wants to send data
				std::cout << "Found a existing connection" << std::endl;
				while (true) {
					readSize = read(eventFD, &readBuffer[0], readBuffer.size());
					if (readSize < 0) {
						std::cout << "Read error" << std::endl;
						// epoll_ctl(epollFD, EPOLL_CTL_DEL, eventFD, NULL);
						createEpollEvent(epollFD, eventFD, EPOLL_CTL_DEL, EPOLLIN);
						close(eventFD);
						break;
					}
					else if (readSize == 0) {
						std::cout << "Found the end of the message" << std::endl;
						std::cout << "Client disconnected" << std::endl;
						// epoll_ctl(epollFD, EPOLL_CTL_DEL, eventFD, NULL);
						createEpollEvent(epollFD, eventFD, EPOLL_CTL_DEL, EPOLLIN);
						close(eventFD);
						break;
					}
					else {
						std::cout << "Received size = " << readSize << std::endl;
						mainBuffer.append(readBuffer.data(), readSize);
						std::cout << "mainBuffer: " << mainBuffer << std::endl;
					}
						
				}
				std::cout << YELLOW << "Total size read: " << mainBuffer.size() << RESET << std::endl;
				std::cout << GREEN << "Read data from client:" << mainBuffer << RESET << std::endl;
			}
		}
	}
		// if ((newSocket = accept(serverFD, res->ai_addr, &res->ai_addrlen)) < 0) {
		// 	perror("accept");
		// 	freeaddrinfo(res);
		// 	return (1);
		// }
	// 	while (running)
	// 	{
	// 		std::cout << PURPLE << eventCount << " events ready" << RESET << std::endl;
	// 		for (int i = 0; i < eventCount; i++) {
	// 			readSize = -2;
	// 			while ((readSize = read(newSocket, &readBuffer[0], readBuffer.size())) > 0) {
	// 				if (readSize < 0) {
	// 					perror("read");
	// 					freeaddrinfo(res);
	// 					return (1);
	// 				}
	// 				std::cout << "Received size = " << readSize << std::endl;
	// 				mainBuffer.append(readBuffer.data(), readSize);
	// 				// std::string responseString = "Hello back";
	// 				// send(newSocket, responseString.c_str(), responseString.size(), 0);
	// 			}
	// 			if (mainBuffer.find("stop") != std::string::npos) {
	// 				std::cout << "Switching running boolean to false" << std::endl;
	// 				running = false;
	// 			}
	// 			std::cout << YELLOW << "Total size read: " << mainBuffer.size() << RESET << std::endl;
	// 			std::cout << GREEN << "Read data from client:" << mainBuffer << RESET << std::endl;
	// 			close(newSocket);
	// 			sleep(3);
	// 		}
	// 	}
	if (close(epollFD)) {
		std::cerr << "Failed to close epoll file descriptor" << std::endl;
		return 1;
	}
	close(serverFD);
	freeaddrinfo(res);
	return (0);
}
