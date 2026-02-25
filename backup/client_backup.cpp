
#include "../include/terminalColors.hpp"
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
	// while (true)
	// {
	// 	if (!std::getline(std::cin, str))
	// 		break;
	// 	if (str == "stop")
	// 		break;
	// 	send(clientFD, str.c_str(), str.size(), 0);
	// }
	std::string request =
        "GET / HTTP/1.1\r\n"         
        "Host: 127.0.0.1\r\n"      
        "Connection: close\r\n"     
        "\r\n";

	if (send(clientFD, request.c_str(), request.size(), 0) == -1) {
        std::cerr << "Erreur send" << std::endl;
    } else {
        std::cout << "Requête envoyée :\n" << request;
    }

	std::string read(BUFFERSIZE, 0);
    ssize_t bytesRead = recv(clientFD, &read[0], readBuffer.size() - 1, 0);
    if (bytesRead > 0) {
        std::cout << "\nRéponse reçue (" << bytesRead << " octets) :\n" << read << std::endl;
    } else if (bytesRead == 0) {
        std::cout << "Connexion fermée par le serveur." << std::endl;
    } else {
        std::cerr << "Erreur recv" << std::endl;
    }

	close(clientFD);
	// read(clientFD, &readBuffer[0], readBuffer.size());

	// std::cout << readBuffer << std::endl;

	freeaddrinfo(res);
}

