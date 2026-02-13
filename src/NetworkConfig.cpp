
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

NetworkConfig::~NetworkConfig() {};

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

bool NetworkConfig::prepareAddressInfo()
{
	struct addrinfo prep;
	std::memset(&prep, 0, sizeof(addrinfo));

	prep.ai_family = AF_INET;
	prep.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo("127.0.0.1", "8080", &prep, &_res);
	if (status != 0)
		throw Tools::CustomException(gai_strerror(status));
	return true;
}

try {
	_netwConf.prepareAddressInfo();
}
catch (Tools::CustomException& e) {
	std::cout << e.what() << std:endl;
}
