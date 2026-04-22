#ifndef SERVER_HPP
#define SERVER_HPP

#include "parsing/ServerBlockConfig.hpp"
#include "TerminalColors.hpp"
#include "LocationConfig.hpp"
#include "ServerSocket.hpp"
#include "ConfigBase.hpp"
#include "Polling.hpp"

#include <csignal>
#include <cstdlib>
#include <cerrno>
#include <set>
#include <map>

class Server : public ConfigBase
{
private:
	ServerSocket *_servSocket;
	std::string _port;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	
	Server &operator=(const Server &obj);
	Server();
	
	LocationConfig _serversLocationConfigBaseWorkaroundBecauseOfArttu;
public:
	Server(const Server &obj);
	Server(std::string ip, const std::string &port);
	Server(const ServerBlockConfig &config, ServerSocket *socket);
	// Server(const ServerBlockConfig &config, const ServerSocket *servSocket);
	~Server();

	const std::string &getPort() const;
	int getServSockFD() const;
	const std::set<std::string> &getServerNames() const;
	const ServerSocket *getServSocket() const;
	const std::map<std::string, LocationConfig> &getLocationConfigs() const;
	const ConfigBase &getPathConfig(std::string &path);
};

#endif