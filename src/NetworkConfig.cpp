#include "NetworkConfig.hpp"

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
