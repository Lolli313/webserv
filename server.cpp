
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
#include <vector>

#define BUFFERSIZE 1
#define MAX_EVENTS 5
#define TIMEOUT -1

/**
 * Register , modify or delete a socket (targetFD) in the epoll intereset list.
 * Types of epoll_events:
 * - EPOLL_CTL_ADD
 * - EPOLL_CTL_MOD = to change the epollEventFlag.
 * - EPOLL_CTL_DEL
 */
bool epollEventAction(int epollFD, int targetFd, int epollEvent, int epollEventFlag) {
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
	int serverFD, readSize, epollFD = -2;
	struct addrinfo prep, *res;
	std::memset(&prep, 0, sizeof(addrinfo));
	std::string readBuffer, mainBuffer;
	
	if ((epollFD = epoll_create1(0)) < 0) {
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		return (1);
	}
	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;

	readBuffer.resize(BUFFERSIZE);
	mainBuffer.resize(BUFFERSIZE);

	int status = 0;
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
	if (!epollEventAction(epollFD, serverFD, EPOLL_CTL_ADD, EPOLLIN)) {
		std::cerr << RED << "Failed to add serverFD to epoll pool" << RESET << std::endl;
		return 1;
	}
	struct epoll_event eventArray[MAX_EVENTS];

//	std::vector<int> clientList;

	int eventCount;
//	while (true) {
		eventCount = epoll_wait(epollFD, eventArray, MAX_EVENTS, TIMEOUT);
		std::cout << PURPLE << eventCount << " events ready" << RESET << std::endl;
		for (int i = 0; i < eventCount; i++) {
			int eventFD = eventArray[i].data.fd;
			if (eventFD == serverFD) { // a new client requests a connection
				std::cout << "Found a new connection" << std::endl;
				int newSocket;
				if ((newSocket = accept(eventFD, res->ai_addr, &res->ai_addrlen)) < 0) {
					freeaddrinfo(res);
					perror("accept");
					return (1);
				}
//				clientList.
				epollEventAction(epollFD, newSocket, EPOLL_CTL_ADD, EPOLLIN);
			}
			else { // old client wants to send data
				std::cout << "Found an existing connection" << std::endl;
				while (true) {
					readSize = read(eventFD, &readBuffer[0], BUFFERSIZE);
					if (readSize < 0) {
						std::cout << "Read error" << std::endl;
						epollEventAction(epollFD, eventFD, EPOLL_CTL_DEL, EPOLLIN);
						close(eventFD);
						break;
					}
					else if (readSize == 0) {
						std::cout << "Found the end of the message" << std::endl;
						std::cout << "Client disconnected" << std::endl;
						epollEventAction(epollFD, eventFD, EPOLL_CTL_DEL, EPOLLIN);
						close(eventFD);
						std::cout << YELLOW << "Total size read: " << mainBuffer.size() << RESET << std::endl;
						std::cout << GREEN << "Read data from client:" << mainBuffer << RESET << std::endl;
						mainBuffer.clear();
						break;
					}
					else {
						std::cout << "Received size = " << readSize << std::endl;
						mainBuffer.append(readBuffer.data(), readSize);
					}			
				}
			}
		}
//	}
	if (close(epollFD)) {
		std::cerr << "Failed to close epoll file descriptor" << std::endl;
		freeaddrinfo(res);
		return 1;
	}
	close(serverFD);
	freeaddrinfo(res);
	return (0);
}
