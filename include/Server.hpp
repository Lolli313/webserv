#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "ServerSocket.hpp"
#include "Polling.hpp"
#include "terminalColors.hpp"

#include <csignal>
#include <cerrno>
#include <set>
#include <map>

class Server
{
private:
	ServerSocket _servSocket;

	std::string _listen;
	std::set<std::string> _server_names;
	std::string _root;
	std::set<std::string> _index;
	bool _autoindex;

	// server block can have multiple location blocks with different paths (map1 key),
	// and each location can have multiple directives (map2 key) that have different values (map2 values)
	// So it's a map(1) with PATH as key, and a map(2) as value, which has DIRECTIVE as key and value as VALUE
	std::map<std::string, std::map<std::string, std::string> > _locations;
	long _clientMaxBodySize;
	std::map<int, std::string> _errorPages;
	
	
	
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