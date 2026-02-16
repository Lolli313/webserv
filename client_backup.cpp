#include "terminalColors.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <string>

#define BUFFERSIZE 1024

int main(int ac, char **av)
{
	(void)ac;
	int clientFD;
	struct addrinfo prep, *res;

	std::string readBuffer;

	readBuffer.resize(BUFFERSIZE);
	std::memset(&prep, 0, sizeof(addrinfo));

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo("127.0.0.1", "8080", &prep, &res);

	(void)status;

	std::string str(av[1]);
	str += " ";
	clientFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	connect(clientFD, res->ai_addr, res->ai_addrlen);
	while (true)
	{
		str += "a";
		send(clientFD, str.c_str(), str.size(), 0);
		sleep(std::atoi(av[2]));
	}

	close(clientFD);
	// read(clientFD, &readBuffer[0], readBuffer.size());

	// std::cout << readBuffer << std::endl;

	freeaddrinfo(res);
}
