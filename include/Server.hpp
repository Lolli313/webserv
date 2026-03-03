#ifndef SERVER_HPP
#define SERVER_HPP

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

// class LocationConfig;

class Server : public ConfigBase
{
private:
	ServerSocket _servSocket;
	int _port;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	
	Server &operator=(const Server &obj);
	Server();
	
	LocationConfig _serversLocationConfigBaseWorkaroundBecauseOfArttu;
public:
	Server(const Server &obj);
	Server(const std::string &port);
	~Server();

	int getPort() const;
	int getServSockFD() const;
	const std::set<std::string> &getServerNames() const;
	const ServerSocket &getServSocket() const;
	const std::map<std::string, LocationConfig> &getLocationConfigs() const;
	const LocationConfig &getPathConfig(const std::string &path);
};

#endif