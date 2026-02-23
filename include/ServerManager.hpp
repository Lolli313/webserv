#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "terminalColors.hpp"
#include "ServerSocket.hpp"
#include "Polling.hpp"
#include "Server.hpp"

#include <vector>

extern int _sigStop = 0;

class ServerManager
{
private:
	std::vector<Server*> _servers;
	std::vector<ServerSocket*> _servSockets;
	Polling _polling;
	ServerManager();
	ServerManager(const ServerManager &obj);
	ServerManager &operator=(const ServerManager &obj);

public:
	ServerManager(std::vector<std::string> ports);
	~ServerManager();

	std::vector<ServerSocket*> setupServSockets(std::vector<std::string> ports);
	void ServerManager::existingClient(unsigned int i, int eventFD);
	bool ServerManager::matchServerFD(int eventFD) const;
	void ServerManager::eventLoop();
	void ServerManager::mainLoop();
};

#endif