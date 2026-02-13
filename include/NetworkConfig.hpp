#ifndef NETWORKCONFIG_CLASS_HPP
#define NETWORKCONFIG_CLASS_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <string>

class NetworkConfig
{
private:
	std::string _ipAddr;
	int _port;
	struct addrinfo *_res;

public:
	NetworkConfig();
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

	bool freeNetworkConfig(struct addrinfo *obj);
};

#endif