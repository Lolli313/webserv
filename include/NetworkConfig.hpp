#ifndef NETWORKCONFIG_CLASS_HPP
#define NETWORKCONFIG_CLASS_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <string>

#define DEFAULT_LOCAL_IP "127.0.0.1"
#define DEFAULT_LOCAL_PORT "8080"

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

	void prepareAddressInfo(const std::string &ipAddr, const std::string &port);
	void freeNetworkConfig();
};

#endif