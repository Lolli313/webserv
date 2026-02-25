#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "ServerSocket.hpp"
#include "Polling.hpp"
#include "terminalColors.hpp"
#include "ConfigBase.hpp"
#include "LocationConfig.hpp"

#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <set>
#include <map>

class Server : public ConfigBase
{
private:
	ServerSocket _servSocket;
	int _port;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	
	Server(const Server &obj);
	Server &operator=(const Server &obj);
	Server();
	
public:	
	Server(const std::string &ports);
	~Server();

	int getPort() const;
	int getServSockFD() const;
	const std::set<std::string> &getServerNames() const;
};

#endif