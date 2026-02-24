#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "ServerSocket.hpp"
#include "Polling.hpp"
#include "terminalColors.hpp"
#include "ConfigBase.hpp"

#include <csignal>
#include <cerrno>
#include <set>
#include <map>

class Server : public ConfigBase
{
private:
	ServerSocket _servSocket;
	std::string _listeningPort;
	std::set<std::string> _serverNames;
	
	Server(const Server &obj);
	Server &operator=(const Server &obj);
	Server();
	
public:	
	Server(const std::string &ports);
	~Server();

	std::vector<ServerSocket*> setupServSockets(std::vector<std::string> ports);
	void pollingSetup();
	void mainLoop();
	void eventLoop();
	void handleSignal(int sig);
	void existingClient(unsigned int i, int eventFD);
	bool matchServerFD(int eventFD) const;
//	void freeServSocket(ServerSocket* tmp);
};

#endif