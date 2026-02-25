#include "Server.hpp"

void freeServSocket(ServerSocket* tmp);

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Server::Server(const std::string &port) : _servSocket(port), _port(std::atoi(port.c_str())) {}

Server::~Server() {
	std::cout << "Calling Server's destructor" << std::endl;
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Server::getPort() const { return _port; }
int Server::getServSockFD() const { return _servSocket.getServSockFD(); }
const std::set<std::string>& Server::getServerNames() const { return _serverNames; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
