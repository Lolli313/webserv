#ifndef NETWORKCONFIG_CLASS_HPP
#define NETWORKCONFIG_CLASS_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <string>
#include "terminalColors.hpp"

#define DEFAULT_LOCAL_IP "0.0.0.0"
#define DEFAULT_LOCAL_PORT "8080"

class NetworkConfig
{
private:
	std::string _ipAddr;
	std::string _port;
	struct addrinfo *_info;

public:
	NetworkConfig();
	NetworkConfig(const std::string &ipAddr, const std::string &port);
	NetworkConfig(const std::string &port);
	NetworkConfig(const NetworkConfig &obj);
	NetworkConfig &operator=(const NetworkConfig &obj);
	~NetworkConfig();

	void setIpAddr(std::string ipAdrr);
	const std::string &getIpAddr() const;

	int getProtocol() const;
	int getFamily() const;
	int getSockType() const;
	struct sockaddr *getAdrr() const;
	socklen_t getAddrLen() const;
	struct addrinfo *getInfo();

	void prepareAddressInfo(const std::string &ipAddr, const std::string &port);
	void freeNetworkConfig();
};

#endif