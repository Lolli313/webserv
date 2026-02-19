#ifndef SERVERSOCKET_CLASS_HPP
#define SERVERSOCKET_CLASS_HPP

#include "NetworkConfig.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "Tools.hpp"
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

// 1 per port
class ServerSocket
{
private:
	int _servSockFD;
	NetworkConfig _netwConf;
	ServerSocket();
	ServerSocket &operator=(const ServerSocket &obj);

public:
	ServerSocket(std::string port);
	ServerSocket(const ServerSocket &obj);
	~ServerSocket();

	int getServSockFD() const;
	NetworkConfig getNetwConf() const;

	void createServerSocket();
	void setSocketOptions();
	void connectSocketToPort();
};

#endif