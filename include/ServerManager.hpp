#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"

#include <vector>

class ServerManager
{
private:
	std::vector<Server*> _servers;

public:
	ServerManager();
	ServerManager(const ServerManager &obj);
	ServerManager &operator=(const ServerManager &obj);
	~ServerManager();

};

#endif