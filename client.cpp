#include "terminalColors.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string>

int main() {
	int clientFD;
	struct addrinfo prep, *res;

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo("127.0.0.1", "8080", &prep, &res);

	(void)status;

	clientFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	connect(clientFD, res->ai_addr, res->ai_addrlen);
	
	std::string str = "Hello!!!!";

	send(clientFD, str.c_str(), str.size(), 0);
}