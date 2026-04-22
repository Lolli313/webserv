#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "NetworkConfig.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "Tools.hpp"
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <fcntl.h>

// 1 per port
class ServerSocket
{
private:
	const std::string _port;
	int _servSockFD;
	NetworkConfig _netwConf;
	ServerSocket();
	ServerSocket &operator=(const ServerSocket &obj);

public:
	ServerSocket(std::string ip, std::string port);
	ServerSocket(const ServerSocket &obj);
	~ServerSocket();

	int getServSockFD() const;
	NetworkConfig getNetwConf() const;
	const std::string &getPort() const;

	void createServerSocket();
	void setSocketOptions();
	void connectSocketToPort();
};

#endif