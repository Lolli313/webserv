
#include "terminalColors.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <vector>

#define BUFFERSIZE 2
#define MAX_EVENTS 5
#define TIMEOUT -1

/**
 * Register , modify or delete a socket (targetFD) in the epoll intereset list.
 * Types of epoll_events:
 * - EPOLL_CTL_ADD
 * - EPOLL_CTL_MOD = to change the epollEventFlag.
 * - EPOLL_CTL_DEL
 */
bool epollEventAction(int epollFD, int targetFd, int epollEvent, int epollEventFlag)
{
	struct epoll_event event;

	event.events = epollEventFlag;
	event.data.fd = targetFd;

	if (epoll_ctl(epollFD, epollEvent, targetFd, &event))
	{
		std::cerr << "Failed to create epoll event" << std::endl;
		// close(epollFD);
		return (false);
	}
	return (true);
}

int main()
{
	int serverFD, readSize, epollFD = -2;
	struct addrinfo prep, *res;
	std::memset(&prep, 0, sizeof(addrinfo));
	std::string readBuffer, mainBuffer;

	if ((epollFD = epoll_create1(0)) < 0)
	{
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		return (1);
	}
	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;

	readBuffer.resize(BUFFERSIZE);
	mainBuffer.resize(BUFFERSIZE);

	int status = 0;
	if ((status = getaddrinfo("127.0.0.1", "8080", &prep, &res)) != 0)
	{
		std::cout << gai_strerror(status) << std::endl;
		std::cout << res->ai_flags << std::endl;
		freeaddrinfo(res);
		return (1);
	}

	if ((serverFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
	{
		perror("socket");
		freeaddrinfo(res);
		return (1);
	}

	int option = 1;
	if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
	{
		perror("setsockeopt REUSEADDR");
		close(serverFD);
		freeaddrinfo(res);
		return (1);
	}
	if (setsockopt(serverFD, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option)) < 0)
	{
		perror("setsockeopt SO_KEEPALIVE");
		close(serverFD);
		freeaddrinfo(res);
		return (1);
	}

	if (fcntl(serverFD, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("fcntl");
		close(serverFD);
		freeaddrinfo(res);
		return (1);
	}

	if (bind(serverFD, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("bind");
		freeaddrinfo(res);
		return (1);
	}

	if (listen(serverFD, 5) < 0)
	{
		perror("listen");
		freeaddrinfo(res);
		return (1);
	}

	if (!epollEventAction(epollFD, serverFD, EPOLL_CTL_ADD, EPOLLIN))
	{
		std::cerr << RED << "Failed to add serverFD to epoll pool" << RESET << std::endl;
		close(epollFD);
		return 1;
	}

	// SERVER LOOP
	struct epoll_event eventArray[MAX_EVENTS];
	int eventCount;
	bool running = true;
	while (running)
	{
		eventCount = epoll_wait(epollFD, eventArray, MAX_EVENTS, TIMEOUT);
		std::cout << PURPLE << eventCount << " events ready" << RESET << std::endl;

		// NEW EVENTS: GO THROUGH EACH EVENT
		for (int i = 0; i < eventCount; i++)
		{
			int eventFD = eventArray[i].data.fd;

			// REGISTER NEW SOCKET / CLIENT
			if (eventFD == serverFD)
			{
				int newSocket;
				sockaddr_in clientAddr;
				socklen_t clientLen = sizeof(clientAddr);

				// ACCEPT ALL THE INCOMING CONNECTIONS
				if ((newSocket = accept(eventFD, (sockaddr *)&clientAddr, &clientLen)) >= 0)
				{
					// SET THE FLAGS OF EVENTS THAT WE WNT TO MONITOR
					std::cout << "Found a new connection" << std::endl;
					int registerEvents = EPOLLIN | EPOLLRDHUP | EPOLLERR;

					fcntl(newSocket, F_SETFL, O_NONBLOCK);
					if (!epollEventAction(epollFD, newSocket, EPOLL_CTL_ADD, registerEvents))
					{
						close(newSocket); // Should we close it ?
						close(eventFD);
						freeaddrinfo(res);
						break;
					}
				}
				// AN ERROR OCCURED
				if (newSocket < 0)
				{
					std::cout << RED << "accept return value = " << newSocket << RESET << std::endl;
					freeaddrinfo(res);
					close(eventFD);
					perror("accept");
					break;
					// return (1);
				}
			}

			// CLIENT EVENT
			else
			{
				std::cout << "Found an existing connection" << std::endl;

				// CLIENT DISCONNECTED
				uint32_t currEvent = eventArray[i].events;
				if (currEvent & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
				{
					std::cout << "Client disconnected or error" << std::endl;
					epollEventAction(epollFD, eventFD, EPOLL_CTL_DEL, 0);
					close(eventFD);
					continue;
				}

				// RECEIVING
				if (currEvent & EPOLLIN)
				{
					if ((readSize = recv(eventFD, &readBuffer[0], BUFFERSIZE, 0)) > 0)
					{
						mainBuffer.append(readBuffer.data(), readSize);
						std::cout << "Received size = " << readSize << std::endl;
						std::cout << GREEN_BRIGHT << "Received content: " << readBuffer << RESET << std::endl;
						std::cout << LIGHT_BLUE << "All content received: " << mainBuffer << RESET << std::endl;
						if (mainBuffer.find("stop") != std::string::npos)
						{
							close(eventFD);
							close(epollFD);
							close(serverFD);
							freeaddrinfo(res);
							return (1);
						}
						std::cout << "NOT FOUND" << std::endl;
					}

					std::cout << RED << "HERE=======================" << RESET << std::endl;

					// ERROR
					if (readSize < 0)
					{
						std::cout << "Rcv error" << std::endl;
						epollEventAction(epollFD, eventFD, EPOLL_CTL_DEL, 0);
						close(eventFD);
					}

					// CLIENT IS DONE SENDING
					else if (readSize == 0)
					{
						std::cout << "Found the end of the message" << std::endl;
						std::cout << "Client disconnected" << std::endl;
						epollEventAction(epollFD, eventFD, EPOLL_CTL_DEL, 0);
						close(eventFD);

						// END OF RECEPTION, PRINT THE OUTPUT OF THE CLIENT
						std::cout << YELLOW << "Total size read: " << mainBuffer.size() << RESET << std::endl;
						std::cout << GREEN << "Read data from client:" << mainBuffer << RESET << std::endl;
						mainBuffer.clear();
						continue;
					}
				}
			}
		}
	}
	if (close(epollFD))
	{
		std::cerr << "Failed to close epoll file descriptor" << std::endl;
		freeaddrinfo(res);
		return 1;
	}
	close(serverFD);
	freeaddrinfo(res);
	return (0);
}