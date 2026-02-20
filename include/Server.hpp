#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "ServerSocket.hpp"
#include "Polling.hpp"
#include "terminalColors.hpp"
#include <csignal>
#include <cerrno>

class Server
{
private:
	std::vector<ServerSocket*> _servSockets;
	Polling _polling;
//	ServerSocket _servSock;
	
	Server(const Server &obj);
	Server &operator=(const Server &obj);
	Server();
	
public:	
	Server(std::vector<std::string> ports);
	~Server();

	std::vector<ServerSocket*> setupServSockets(std::vector<std::string> ports);
	void pollingSetup();
	void mainLoop();
	void eventLoop();
	void handleSignal(int sig);
	void existingClient(unsigned int i, int eventFD);
	bool matchServerFD(int eventFD) const;
//	void freeServSocket(ServerSocket* tmp);
};

#endif