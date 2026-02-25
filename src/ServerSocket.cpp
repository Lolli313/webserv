
#include "ServerSocket.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

// Exception on failure
// ServerSocket::ServerSocket()
// {
// 	createServerSocket();
// 	setSocketOptions();
// }

// Exception on failure
ServerSocket::ServerSocket(std::string port) : _servSockFD(-1), _netwConf(NetworkConfig(port)) {
	createServerSocket();
	setSocketOptions();
	connectSocketToPort();
}

ServerSocket::~ServerSocket() { 
	std::cout << RED << "ServerSocket destructor" << RESET << std::endl;
	// if (_servSockFD != -1)
	// 	close(_servSockFD); 
	}

ServerSocket::ServerSocket(const ServerSocket &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

ServerSocket &ServerSocket::operator=(const ServerSocket &obj)
{
	(void)obj;
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int ServerSocket::getServSockFD() const { return _servSockFD; }
NetworkConfig ServerSocket::getNetwConf() const { return _netwConf; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

// Exception on failure
void ServerSocket::createServerSocket()
{
	std::cout << _netwConf.getFamily() << " and " << _netwConf.getSockType() << " and " << _netwConf.getProtocol() << std::endl;
	_servSockFD = socket(_netwConf.getFamily(), _netwConf.getSockType(), _netwConf.getProtocol());
	if (_servSockFD < 0)
		throw Tools::Exception("createServerSocket");
}

// Exception on failure
void ServerSocket::setSocketOptions()
{
	int option = 1;
	std::cout << PURPLE << "servsockFD" << _servSockFD << RESET << std::endl;
	if (setsockopt(_servSockFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
		throw Tools::Exception("setsockeopt REUSEADDR");
	if (setsockopt(_servSockFD, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option)) < 0)
		throw Tools::Exception("setsockeopt SO_KEEPALIVE");
	if (fcntl(_servSockFD, F_SETFL, O_NONBLOCK) < 0)
		throw Tools::Exception("fcntl");
}

// Exception on failure
void ServerSocket::connectSocketToPort()
{
	std::cout << "HELLO BIND" << std::endl;
	if (bind(_servSockFD, _netwConf.getAdrr(), _netwConf.getAddrLen()) < 0)
		throw Tools::Exception("bind");

	// MAGIC NUMBER TO BE SET WITH MAX_EVENTS
	if (listen(_servSockFD, 5) < 0)
		throw Tools::Exception("listen");
}
