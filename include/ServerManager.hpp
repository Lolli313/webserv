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
	std::vector<Server> serverArray; // To store the servers, that will be retrieved throw the following map
	std::map<std::pair<int /*port*/, std::string /*serverName*/>, Server*> _serversMap; // map<pair<port, serverName>, Sever *>
	std::set<int> _servSockFDs; // fd as para, to know if the fd is a server one.
	Polling _polling;
	ServerManager();
	ServerManager(const ServerManager &obj);
	ServerManager &operator=(const ServerManager &obj);

public:
	ServerManager(std::vector<std::string> ports);
	// ServerManager(ParseConfig); // Constructor with
	~ServerManager();

	void existingClient(unsigned int i, int eventFD);
	bool matchServerFD(int eventFD) const;
	void eventLoop();
	void mainLoop();
};

#endif