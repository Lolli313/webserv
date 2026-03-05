
#include "../include/terminalColors.hpp"
#include "../include/HttpTools.hpp"
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
#define PORT "8080"

int main()
{
	int clientFD;
	struct addrinfo prep, *res;

	std::string readBuffer;

	readBuffer.resize(BUFFERSIZE);
	std::memset(&prep, 0, sizeof(addrinfo));

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo("127.0.0.1", PORT, &prep, &res);

	(void)status;

	clientFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	std::cout << clientFD << std::endl;
	std::cout << connect(clientFD, res->ai_addr, res->ai_addrlen) << std::endl;
	std::string str;
	int readSize = 0;
	while (std::getline(std::cin, str))
	{
		// Remove trailing \r if present (from CRLF line endings)
		if (!str.empty() && str.back() == '\r')
			str.pop_back();

		if (str == "stop")
			break;

		send(clientFD, str.c_str(), str.size(), 0);

		std::string response;
		while ((readSize = read(clientFD, &readBuffer[0], readBuffer.size())) > 0)
		{
			response.append(readBuffer.data(), readSize);

			if (readSize < (ssize_t)readBuffer.size())
				break; // likely end of current message
		}

		// Strip all CRLF sequences from the complete response
		std::size_t pos;
		while ((pos = response.find(CRLF)) != std::string::npos)
		{
			response.erase(pos, 2);
		}

		std::cout << response << std::endl;
	}

	close(clientFD);

	freeaddrinfo(res);
}
