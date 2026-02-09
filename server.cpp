
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <cstring>
#include "terminalColors.hpp"

int main() {
	int serverFD, newSocket, readSize;
	struct addrinfo prep, *res;
	std::memset(&prep, 0, sizeof(addrinfo));
	// std::memset(res, 0, sizeof(addrinfo));
	std::string readBuffer("");
	int status = 0;

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	if ((status = getaddrinfo("127.0.0.1", "8080", &prep, &res)) != 0)
	{
		std::cout << gai_strerror(status) << std::endl;
		std::cout << res->ai_flags << std::endl;
	}
	
	serverFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	bind(serverFD, res->ai_addr, res->ai_addrlen);

	listen(serverFD, 5);
	
	newSocket = accept(serverFD, res->ai_addr, &res->ai_addrlen);

	while (1) {
		readSize = -2;
		readSize = read(newSocket, &readBuffer[0], readBuffer.size());
		std::cout << "Received size = " << readSize << std::endl;
		std::cout << GREEN << "Read data from client:" << readBuffer << RESET << std::endl;
		std::string responseString = "Hello back";
		send(newSocket, responseString.c_str(), responseString.size(), 0);
	}
	return (0);
}