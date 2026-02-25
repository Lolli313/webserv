#include "Server.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Server::Server(const std::string &port) : _servSocket(port), _port(std::atoi(port.c_str())) {
	std::cout << GREEN << "Server constructor for _servSocketFD = " << _servSocket.getServSockFD() << std::endl;
	std::cout  << "Server constructor for _port = " << _port << RESET << std::endl;
}

Server::Server(const Server &obj) :
	_servSocket(obj.getServSocket()),
	_port(obj.getPort()),
	_serverNames(obj.getServerNames()),
	_locationConfigs(obj.getLocationConfigs()) 
	{
		std::cout << BLUE << "Server copy constructor" << RESET << std::endl;
	}

Server::~Server() {
	std::cout << RED << "Calling Server's destructor" << RESET << std::endl;
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Server::getPort() const { return _port; }
int Server::getServSockFD() const { return _servSocket.getServSockFD(); }
const std::set<std::string>& Server::getServerNames() const { return _serverNames; }
const ServerSocket &Server::getServSocket() const { return _servSocket; }
const std::map<std::string, LocationConfig>& Server::getLocationConfigs() const { return _locationConfigs; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
