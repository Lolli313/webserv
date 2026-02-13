#include "ServerSocket.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
ServerSocket::ServerSocket() {};

ServerSocket::~ServerSocket() {};

ServerSocket::ServerSocket(const ServerSocket &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

ServerSocket &ServerSocket::operator=(const ServerSocket &obj)
{
	if (this != &obj)
	{
	}
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int ServerSocket::getServerFD() const { return serverFD; };

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool ServerSocket::createServerSocket()
{
}
bool ServerSocket::setSocketOptions()
{
}
bool ServerSocket::connectSocketToPort()
{
}