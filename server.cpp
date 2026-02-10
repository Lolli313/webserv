
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

#define BUFFERSIZE 20

int main() {
	int serverFD, newSocket, readSize;
	struct addrinfo prep, *res;
	struct sockaddr_in address;
	std::memset(&prep, 0, sizeof(addrinfo));
	// std::memset(res, 0, sizeof(addrinfo));
	std::string readBuffer;
	// char readBuffer[BUFFERSIZE] = "";
	int status = 0;

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	readBuffer.resize(BUFFERSIZE);
	// readBuffer.reserve(BUFFERSIZE);
	// std::cout << "Cap before = " << readBuffer.capacity() << std::endl;
	// std::cout << "Cap after = " << readBuffer.capacity() << std::endl;
	
	if ((status = getaddrinfo("127.0.0.1", "8080", &prep, &res)) != 0)
	{
		std::cout << gai_strerror(status) << std::endl;
		std::cout << res->ai_flags << std::endl;
		freeaddrinfo(res);
		return (1);
	}
	
	if ((serverFD = socket(address.sin_family, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		freeaddrinfo(res);
		return (1);
	}

	if (bind(serverFD, (struct sockaddr*)&address, sizeof(address)) < 0)
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

	socklen_t addrlen = sizeof(address);
	
	for (int i = 0; i < 6; i ++) {
		if ((newSocket = accept(serverFD, (struct sockaddr*)&address, &addrlen)) < 0)
		{
			perror("accept");
			freeaddrinfo(res);
			return (1);
		}
		readSize = -2;
		if ((readSize = read(newSocket, &readBuffer[0], readBuffer.size())) < 0)
		{
			perror("read");
			freeaddrinfo(res);
			return (1);
		}
		std::cout << "Received size = " << readSize << std::endl;
		std::cout << GREEN << "Read data from client:" << readBuffer << RESET << std::endl;
		// std::string responseString = "Hello back";
		// send(newSocket, responseString.c_str(), responseString.size(), 0);
		close(newSocket);
	}
	close(serverFD);
	freeaddrinfo(res);
	return (0);
}