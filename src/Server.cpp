#include "Server.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Server::Server(const std::string &port) : ConfigBase(*this), _servSocket(port), _port(port)
{
	std::cout << GREEN << "Server constructor for _servSocketFD = " << _servSocket.getServSockFD() << std::endl;
	std::cout << "Server constructor for _port = " << _port << RESET << std::endl;
}

Server::Server(const Server &obj) :
	ConfigBase(*this),
	_servSocket(obj.getServSocket()),
	_port(obj.getPort()),
	_serverNames(obj.getServerNames()),
	_locationConfigs(obj.getLocationConfigs())
{
	std::cout << BLUE << "Server copy constructor" << RESET << std::endl;
}

Server::Server(const ServerBlockConfig &config) : 
	_servSocket(config.getPort()), 
	_port(config.getPort()), 
	_serverNames(config.getServerNames()), 
	_locationConfigs(config.getLocationConfigs())
	{
		std::cout << BLUE << "Server constructor for _serverSocketFD = " << _servSocket.getServSockFD() <<
			", port = " << _port << RESET << std::endl;
	}


Server::Server(const ServerBlockConfig &config, const ServerSocket &servSocket) :
	_servSocket(servSocket),
	_port(config.getPort()),
	_serverNames(config.getServerNames()),
	_locationConfigs(config.getLocationConfigs()) {
		std::cout << "Server constructor when port is a duplicate: _servSocketFD = " << _servSocket.getServSockFD() <<
			", port = " << _port << RESET << std::endl;
	}

Server::~Server()
{
	std::cout << RED << "Calling Server's destructor" << RESET << std::endl;
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const std::string &Server::getPort() const { return _port; }
int Server::getServSockFD() const { return _servSocket.getServSockFD(); }
const std::set<std::string> &Server::getServerNames() const { return _serverNames; }
const ServerSocket &Server::getServSocket() const { return _servSocket; }
const std::map<std::string, LocationConfig> &Server::getLocationConfigs() const { return _locationConfigs; }

/** @brief Access directly to the path's config, abstracting all the different locationConfigs and the Server's.
 * @return A LocationConfig reference, so all the methods and data are directly accessible. */
const LocationConfig &Server::getPathConfig(const std::string &path)
{
	std::map<std::string, LocationConfig>::const_iterator it = _locationConfigs.find(path);
	if (it != _locationConfigs.end())
		return it->second;
	_serversLocationConfigBaseWorkaroundBecauseOfArttu = static_cast<const ConfigBase &>(*this);
	return (_serversLocationConfigBaseWorkaroundBecauseOfArttu);
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
