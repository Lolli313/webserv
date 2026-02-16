#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "ServerSocket.hpp"
#include "Polling.hpp"

class Server
{
private:
	Polling polling;
	ServerSocket servSock;

public:
	Server();
	Server(const Server &obj);
	Server &operator=(const Server &obj);
	~Server();

	void pollingSetup()
};

#endif
