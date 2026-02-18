#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "ServerSocket.hpp"
#include "Polling.hpp"
#include "terminalColors.hpp"

class Server
{
private:
	std::vector<ServerSocket> _servSockets;
	Polling _polling;
//	ServerSocket _servSock;
	
	Server(const Server &obj);
	Server &operator=(const Server &obj);
	Server();
	
public:	
	Server(std::vector<std::string> ports);
	~Server();

	std::vector<ServerSocket> setupServSockets(std::vector<std::string> ports);
	void pollingSetup();
	void mainLoop();
	void eventLoop();
	void existingClient(unsigned int i, int eventFD);
	bool matchServerFD(int eventFD) const;
};

#endif
