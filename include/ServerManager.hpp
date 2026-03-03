#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "terminalColors.hpp"
#include "parsing/ParseConfig.hpp"
#include "ServerSocket.hpp"
#include "Polling.hpp"
#include "Server.hpp"
#include "HttpResponse.hpp"
#include "MethodGET.hpp"
#include "HttpRequest.hpp"

#include <vector>

extern int _sigStop;

class ServerManager
{
private:
	std::vector<Server> _serverArray;													 // To store the servers, that will be retrieved throw the following map
	std::map<std::pair<int /*port*/, std::string /*serverName*/>, Server *> _serversMap; // map<pair<port, serverName>, Server *>
	std::set<int> _servSockFDs;															 // fd as para, to know if the fd is a server one.
	Polling _polling;
	ServerManager();
	ServerManager(const ServerManager &obj);
	ServerManager &operator=(const ServerManager &obj);

	void handleClientRequest(int clientFD);

public:
	ServerManager(const std::vector<std::string> &ports);
	// ServerManager(ParseConfig); // Constructor with
	~ServerManager();

	// std::vector<Server> setupServers(const std::vector<std::string>& ports);
	std::set<int> setupServSockFDs();
	void existingClient(unsigned int i, int eventFD);
	bool matchServerFD(int eventFD) const;
	void eventLoop();
	void mainLoop();
};

#endif