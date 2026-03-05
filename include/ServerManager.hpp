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
	std::vector<Server> _serverArray; // To store the servers, that will be retrieved throw the following map

	// map<pair<port, serverName &>, Server &> : We create a new key, value for each serverName of a server.
	// If it has 3 names, them the map will have 3 entries for each of its combination <port, serverName>
	//
	// If insert fails, it should not stop the program but just print a log.
	std::map<std::pair<int, std::string>, Server*> _serversMap;

	std::set<int> _servSockFDs; // fd as para, to know if the fd is a server one.
	Polling _polling;
	ServerManager();
	ServerManager(const ServerManager &obj);
	ServerManager &operator=(const ServerManager &obj);

	void handleClientRequest(int clientFD);

public:
	// ServerManager(const std::vector<std::string> &ports);
	ServerManager(const std::vector<ServerBlockConfig> &serverConfigs);
	// ServerManager(ParseConfig); // Constructor with
	~ServerManager();

	std::map<std::pair<int, std::string>, Server*> setupServersMap();
	std::set<int> setupServSockFDs();
	void existingClient(unsigned int i, int eventFD);
	bool matchServerFD(int eventFD) const;
	void eventLoop();
	void mainLoop();
};

#endif