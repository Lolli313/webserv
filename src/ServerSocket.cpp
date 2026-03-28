
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
ServerSocket::ServerSocket(std::string port) : _port(port), _servSockFD(-1), _netwConf(NetworkConfig(port)) {
	try 
	{
		createServerSocket();
		setSocketOptions();
		connectSocketToPort();
	}
	catch (Tools::Exception &e)
	{
		if (_servSockFD != -1)
			close(_servSockFD); 
		throw;
	}
}

ServerSocket::~ServerSocket() { 
	LOG(INFO, RED_BRIGHT, "ServerSocket destructor");
	if (_servSockFD != -1)
		close(_servSockFD); 
	}

ServerSocket::ServerSocket(const ServerSocket &obj) :
	_port(obj.getPort()),
	_servSockFD(obj.getServSockFD()),
	_netwConf(obj.getNetwConf()) 
	{
		LOG(INFO, BLUE, "ServerSocket copy constructor");
	}
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
const std::string &ServerSocket::getPort() const { return _port; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

// Exception on failure
void ServerSocket::createServerSocket()
{
	LOG(INFO, LIGHT_BLUE, Tools::intToString(_netwConf.getFamily()) + " and " + Tools::intToString(_netwConf.getSockType()) +
		" and " + Tools::intToString(_netwConf.getProtocol()));

	_servSockFD = socket(_netwConf.getFamily(), _netwConf.getSockType(), _netwConf.getProtocol());
	LOG(INFO, PINK, "servSocketFD inside ServerSocket class is", Tools::intToString(_servSockFD));
	if (_servSockFD < 0)
		throw Tools::Exception("createServerSocket");
}

// Exception on failure
void ServerSocket::setSocketOptions()
{
	int option = 1;
	LOG(INFO, PURPLE, "servsockFD", Tools::intToString(_servSockFD));
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
	LOG(INFO, LIGHT_GRAY, "HELLO BIND");
	if (bind(_servSockFD, _netwConf.getAdrr(), _netwConf.getAddrLen()) < 0)
		throw Tools::Exception("bind");

	// MAGIC NUMBER TO BE SET WITH MAX_EVENTS
	if (listen(_servSockFD, 5) < 0)
		throw Tools::Exception("listen");
}
