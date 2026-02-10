
#include "terminalColors.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <cstring>
#include <string>

#define BUFFERSIZE 1

int main() {
	int serverFD, newSocket, readSize;
	struct addrinfo prep, *res;
	struct sockaddr_in address;
	std::memset(&prep, 0, sizeof(addrinfo));
	std::string readBuffer;
	std::string mainBuffer;
	int status = 0;

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	readBuffer.resize(BUFFERSIZE);
	mainBuffer.resize(BUFFERSIZE);

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

	if ((newSocket = accept(serverFD, res->ai_addr, &res->ai_addrlen)) < 0)
	{
		perror("accept");
		freeaddrinfo(res);
		return (1);
	}
	
	for (int i = 0; i < 1; i ++) {
		
		// EPOLL
		readSize = -2;
		while ((readSize = read(newSocket, &readBuffer[0], readBuffer.size())) > 0)
		{
			if (readSize < 0)
			{
				perror("read");
				freeaddrinfo(res);
				return (1);
			}
			std::cout << "Received size = " << readSize << std::endl;
			mainBuffer.append(readBuffer.data(), readSize);
			// std::string responseString = "Hello back";
			// send(newSocket, responseString.c_str(), responseString.size(), 0);
		}
		std::cout << YELLOW << "Total size read: " << mainBuffer.size() << RESET << std::endl;
		std::cout << GREEN << "Read data from client:" << mainBuffer << RESET << std::endl;
		close(newSocket);
	}
	close(serverFD);
	freeaddrinfo(res);
	return (0);
}