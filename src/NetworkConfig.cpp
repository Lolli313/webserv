
#include "Tools.hpp"
#include "NetworkConfig.hpp"
#include <cstring>
#include <iostream>
#include <exception>

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
NetworkConfig::NetworkConfig() : _ipAddr(DEFAULT_LOCAL_IP), _port(DEFAULT_LOCAL_PORT)
{
	prepareAddressInfo(_ipAddr, _port);
}

NetworkConfig::NetworkConfig(const std::string &ipAddr, const std::string &port) : _ipAddr(ipAddr), _port(port)
{
	prepareAddressInfo(_ipAddr, _port);
}

NetworkConfig::NetworkConfig(const std::string &port) : _ipAddr(DEFAULT_LOCAL_IP), _port(port)
{
	std::cout << GREEN << "netwowrkconfig constructor" << RESET << std::endl;
	prepareAddressInfo(_ipAddr, _port);
}

NetworkConfig::~NetworkConfig() {
	freeNetworkConfig(); 
	std::cout << RED << "netwowrkconfig destructor" << RESET << std::endl;
}

NetworkConfig::NetworkConfig(const NetworkConfig &obj) :
	_ipAddr(obj.getIpAddr()),
	_port(obj.getPort())
	{
		prepareAddressInfo(_ipAddr, _port);
		std::cout << BLUE << "NetworkConfig copy constructor" << RESET << std::endl;
	}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
NetworkConfig &NetworkConfig::operator=(const NetworkConfig &obj)
{
	if (this != &obj)
	{
		if (_info)
		{
			freeaddrinfo(_info);
		}
		_ipAddr = obj._ipAddr;
		_port = obj._port;
		prepareAddressInfo(_ipAddr, _port);
	}
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

struct addrinfo *NetworkConfig::getInfo() const { return _info; }

// Exeption on failure.
void NetworkConfig::setIpAddr(std::string ipAddr) { _ipAddr = ipAddr; }

const std::string &NetworkConfig::getIpAddr() const { return _ipAddr; }

int NetworkConfig::getProtocol() const { return _info->ai_protocol; }

int NetworkConfig::getFamily() const { return _info->ai_family; }

int NetworkConfig::getSockType() const { return _info->ai_socktype; }

struct sockaddr *NetworkConfig::getAdrr() const { return _info->ai_addr; }

socklen_t NetworkConfig::getAddrLen() const { return _info->ai_addrlen; }

const std::string &NetworkConfig::getPort() const { return _port; }
/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

// Exception on failure.
void NetworkConfig::prepareAddressInfo(const std::string &ipAddr, const std::string &port)
{
	struct addrinfo prep;
	std::memset(&prep, 0, sizeof(addrinfo));

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	std::cout << RED << ipAddr.c_str() << " and " << port.c_str() << RESET << std::endl;
	int status = getaddrinfo(ipAddr.c_str(), port.c_str(), &prep, &_info);
	if (status != 0)
		throw Tools::Exception(gai_strerror(status));
}

void NetworkConfig::freeNetworkConfig() { freeaddrinfo(this->_info); }
