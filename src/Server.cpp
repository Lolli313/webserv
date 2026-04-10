#include "Server.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Server::Server(const std::string &port) : ConfigBase(*this), _servSocket(new ServerSocket(port)), _port(port)
{
	LOG(INFO, "Server constructor for _port = " + _port);
}

Server::Server(const Server &obj) :
	ConfigBase(*this),
	_servSocket(new ServerSocket(*obj._servSocket)),
	_port(obj.getPort()),
	_serverNames(obj.getServerNames()),
	_locationConfigs(obj.getLocationConfigs())
{
	LOG(INFO, BLUE, "Server copy constructor");
}

Server::Server(const ServerBlockConfig &config, ServerSocket *socket) : 
	ConfigBase(config),
	_servSocket(socket), 
	_port(config.getPort()), 
	_serverNames(config.getServerNames()), 
	_locationConfigs(config.getLocationConfigs()) {}


Server::~Server()
{
	LOG(INFO, RED_BRIGHT, "Calling Server's destructor");
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const std::string &Server::getPort() const { return _port; }
int Server::getServSockFD() const { return _servSocket->getServSockFD(); }
const std::set<std::string> &Server::getServerNames() const { return _serverNames; }
const ServerSocket *Server::getServSocket() const { return _servSocket; }
const std::map<std::string, LocationConfig> &Server::getLocationConfigs() const { return _locationConfigs; }

bool locationMatchesPath(std::string &path, const std::string &location)
{
	if (path == location)
		return true;

	// check if a path with a trailing / matches a location path
	if (!path.empty() && path.size() - 1 == location.size() && Tools::getLastCharacter(path) == '/')
		return path.compare(0, location.size(), location) == 0;

	return false;
}

/** @brief Access directly to the path's config, abstracting all the different locationConfigs and the Server's.
 * @return A LocationConfig reference, so all the methods and data are directly accessible.
 */
const ConfigBase &Server::getPathConfig(std::string &path)
{
	LOG(INFO, LIGHT_BLUE, "Path to look for is: ", path);
	while (!path.empty())
	{
		std::map<std::string, LocationConfig>::const_iterator it = _locationConfigs.begin();
		for (; it != _locationConfigs.end(); it++)
		{
			if (locationMatchesPath(path, it->first))
			{
				LOG(INFO, LIGHT_BLUE, "Found a match for the location: ", it->first);
				return it->second;
			}
		}
		if (Tools::getLastCharacter(path) == '/')
			Tools::removeLastCharacter(path);

		Tools::eraseAfterLastCharacter(path, '/');
	}
	return *this;
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
