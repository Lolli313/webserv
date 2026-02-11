#include "terminalColors.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <string>

#define BUFFERSIZE 1024

int main() {
	int clientFD;
	struct addrinfo prep, *res;

	std::string readBuffer;

	readBuffer.resize(BUFFERSIZE);
	std::memset(&prep, 0, sizeof(addrinfo));

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo("127.0.0.1", "8080", &prep, &res);

	(void)status;

	
	
	std::string str = "Hello!!!!";
	
	while (true) {
		clientFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		connect(clientFD, res->ai_addr, res->ai_addrlen);
		send(clientFD, str.c_str(), str.size(), 0);
		close(clientFD);
		sleep(4);
	}

	// read(clientFD, &readBuffer[0], readBuffer.size());

	// std::cout << readBuffer << std::endl;

	freeaddrinfo(res);
}
