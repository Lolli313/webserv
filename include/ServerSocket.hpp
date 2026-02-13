#ifndef SERVERSOCKET_CLASS_HPP
#define SERVERSOCKET_CLASS_HPP

#include "NetworkConfig.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

class ServerSocket
{
private:
	int _serverFD;
	NetworkConfig _netwConf;

public:
	ServerSocket();
	ServerSocket(const ServerSocket &obj);
	ServerSocket &operator=(const ServerSocket &obj);
	~ServerSocket();

	int getServerFD() const;

	bool createServerSocket();
	bool setSocketOptions();
	bool connectSocketToPort();
};

#endif