#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "parsing/ParseConfig.hpp"
#include "TerminalColors.hpp"
#include "ServerSocket.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Polling.hpp"
#include "Server.hpp"
#include "Cookie.hpp"
#include "Delete.hpp"
#include "Post.hpp"
#include "Get.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#define ERROR_PAGE_TEMPLATE_PATH "/files/error_pages/ErrorTemplate.html"
#define TEMPLATE_ERROR_CODE "{(CODE)}"
#define TEMPLATE_ERROR_MESSAGE "{(MSG)}"

extern int _sigStop;

class ServerManager
{
private:

	Cookie _cookie;

	std::vector<Server *> _serverArray; // To store the servers, that will be retrieved throw the following map

	std::vector<ServerSocket *> _serverSocketArray;

	std::map<CGI*, Client*> _CGImap;

	// map<pair<port, serverName &>, Server &> : We create a new key, value for each serverName of a server.
	// If it has 3 names, them the map will have 3 entries for each of its combination <port, serverName>
	//
	// If insert fails, it should not stop the program but just print a log.
	std::map<std::pair<int, std::string>, Server*> _serversMap;

	std::set<int> _servSockFDs; // fd as para, to know if the fd is a server one.
	Polling *_polling;
	ServerManager();
	ServerManager(const ServerManager &obj);
	ServerManager &operator=(const ServerManager &obj);

public:
	// ServerManager(const std::vector<std::string> &ports);
	ServerManager(const std::vector<ServerBlockConfig> &serverConfigs);
	// ServerManager(ParseConfig); // Constructor with
	~ServerManager();

	const std::string execute(const HttpRequest &request, const ConfigBase *config, Client *client);

	void setupServers(const std::vector<ServerBlockConfig> &serverConfigs);
	std::map<std::pair<int, std::string>, Server*> setupServersMap();
	std::set<int> setupServSockFDs();
	const std::string& findPort(int eventFD);
	Server* findServer(const std::string& host, const std::string& port, HttpRequest& request);
	Server* findServer(const std::string& host, const std::string& port);
	void checkRequestValidity(const Client &client, const HttpRequest &httpRequest, int eventFD);
	const ConfigBase *findConfigBase(Client &client, HttpRequest &request);
	long findMaxBodySize(const Client *client, const std::string& host, std::string path);
	std::string requestPreParsing(Client *client);
	void existingClient(Client *client);
	bool matchServerFD(int eventFD) const;
	void eventLoop();
	void mainLoop();
	void sendResponse(Client *client);
	void throwHandler(Client *client, Tools::Exception &e, const ConfigBase *config, bool reThrow);
	void handleResponse(Client *client);
	void setResponseAndDeleteCGI(int eventFD, const std::pair<CGI *, Client*> &it);
	void router(int eventFD);

	void handleTimeout();
};

#endif