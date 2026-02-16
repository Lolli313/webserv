
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
NetworkConfig::NetworkConfig() {};

NetworkConfig::~NetworkConfig() { freeNetworkConfig(); }

NetworkConfig::NetworkConfig(const NetworkConfig &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
NetworkConfig &NetworkConfig::operator=(const NetworkConfig &obj)
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

/*
Exeption on failure.
*/
void NetworkConfig::setIpAddr(std::string ipAddr) { _ipAddr = ipAddr; }

const std::string &NetworkConfig::getIpAddr() const { return _ipAddr; }

int NetworkConfig::getProtocol() const { return _res->ai_protocol; }

int NetworkConfig::getFamily() const { return _res->ai_family; }

int NetworkConfig::getSockType() const { return _res->ai_socktype; }

struct sockaddr *NetworkConfig::getAdrr() const { return _res->ai_addr; }

socklen_t NetworkConfig::getAddrLen() const { return _res->ai_addrlen; }
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
	int status = getaddrinfo(ipAddr.c_str(), port.c_str(), &prep, &_res);
	if (status != 0)
		throw Tools::Exception(gai_strerror(status));
}

void NetworkConfig::freeNetworkConfig() { freeaddrinfo(this->_res); }
