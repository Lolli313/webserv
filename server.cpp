
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

#define BUFFERSIZE 4096
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
		close(epollFD);
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
		perror("setsockeopt");
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
		return 1;
	}
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
				while ((newSocket = accept(eventFD, (sockaddr *)&clientAddr, &clientLen)) < 0)
				{
					// SET THE FLAGS OF EVENTS THAT WE WANT TO MONITOR
					std::cout << "Found a new connection" << std::endl;
					int events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLERR;
					epollEventAction(epollFD, newSocket, EPOLL_CTL_ADD, events);
				}
				// AN ERROR OCCURED
				if (newSocket < 0)
				{
					freeaddrinfo(res);
					perror("accept");
					return (1);
				}
			}

			// CLIENT EVENT
			else
			{
				std::cout << "Found an existing connection" << std::endl;

				// USE RCV() INSTEAD
				readSize = read(eventFD, &readBuffer[0], BUFFERSIZE);
				std::cout << RED << "HERE=======================" << RESET << std::endl;

				// ERROR
				if (readSize < 0)
				{
					std::cout << "Read error" << std::endl;
					epollEventAction(epollFD, eventFD, EPOLL_CTL_DEL, EPOLLIN);
					close(eventFD);
				}

				// CLIENT DECONEXION
				else if (readSize == 0) // Add other mean to detect client deconnection
				{
					std::cout << "Found the end of the message" << std::endl;
					std::cout << "Client disconnected" << std::endl;
					epollEventAction(epollFD, eventFD, EPOLL_CTL_DEL, EPOLLIN);
					close(eventFD);

					// END OF RECEPTION, PRINT THE OUTPUT OF THE CLIENT
					std::cout << YELLOW << "Total size read: " << mainBuffer.size() << RESET << std::endl;
					std::cout << GREEN << "Read data from client:" << mainBuffer << RESET << std::endl;

					// HARDCODED IMPLEMENTATION TO STOP THE SERVER
					if (!mainBuffer.find("stop"))
						running = false;
					mainBuffer.clear();
					break;
				}

				// NORMAL LOGIC
				else
				{
					std::cout << "Received size = " << readSize << std::endl;
					mainBuffer.append(readBuffer.data(), readSize);
					// }
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
