
#include "ServerManager.hpp"
#include "HttpMethod.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ServerManager::~ServerManager()
{
	std::cout << RED << "Calling ServerManager's destructor" << RESET << std::endl;
	for (std::vector<ServerSocket *>::iterator it = _serverSocketArray.begin(); it != _serverSocketArray.end(); it++)
		delete (*it);
	for (std::vector<Server *>::iterator it = _serverArray.begin(); it != _serverArray.end(); it++)
		delete (*it);
	if (_polling)
		delete _polling;
}

ServerManager::ServerManager(const std::vector<ServerBlockConfig> &serverConfigs) : _polling(NULL)
{
	try
	{
		setupServers(serverConfigs);
		_serversMap = setupServersMap();
		_servSockFDs = setupServSockFDs();
		_polling = new Polling(_servSockFDs);
	}
	catch (Tools::Exception &e)
	{
		for (std::vector<ServerSocket *>::iterator it = _serverSocketArray.begin(); it != _serverSocketArray.end(); it++)
			delete (*it);
		for (std::vector<Server *>::iterator it = _serverArray.begin(); it != _serverArray.end(); it++)
			delete (*it);
		if (_polling)
			delete _polling;
		throw;
	}
}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
ServerManager &ServerManager::operator=(const ServerManager &obj)
{
	(void)obj;
	return (*this);
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

std::map<std::pair<int, std::string>, Server *> ServerManager::setupServersMap()
{
	std::map<std::pair<int, std::string>, Server *> tmpMap;
	std::vector<Server *>::iterator it = _serverArray.begin();
	for (; it != _serverArray.end(); it++)
	{
		std::set<std::string>::const_iterator sit = (*it)->getServerNames().begin();
		for (; sit != (*it)->getServerNames().end(); sit++)
			tmpMap.insert(std::make_pair(std::make_pair(std::atoi((*it)->getPort().c_str()), *sit), (*it)));
	}
	return tmpMap;
}

void ServerManager::setupServers(const std::vector<ServerBlockConfig> &serverConfigs)
{
	bool found = false;
	for (std::vector<ServerBlockConfig>::const_iterator mit = serverConfigs.begin(); mit != serverConfigs.end(); mit++)
	{
		std::vector<ServerSocket *>::iterator it = _serverSocketArray.begin();
		for (; it != _serverSocketArray.end(); it++)
		{
			if ((*it)->getPort() == mit->getPort())
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			// Create the socket here
			_serverSocketArray.push_back(new ServerSocket(mit->getPort()));
			_serverArray.push_back(new Server(*mit, _serverSocketArray.back()));
		}
		else
		{
			std::cout << "ALREADY EXISTING SOCKET" << std::endl;
			_serverArray.push_back(new Server(*mit, *it));
		}
		found = false;
		// std::cout << CYAN_BRIGHT << "setupServers for fd = " << mit->getServSockFD() << RESET << std::endl;
	}
}

std::set<int> ServerManager::setupServSockFDs()
{
	std::set<int> tempServSockFDs;
	std::vector<Server *>::const_iterator it = _serverArray.begin();
	for (; it != _serverArray.end(); it++)
	{
		std::cout << YELLOW_BRIGHT << "setupServSockFDs for fd = " << (*it)->getServSockFD() << RESET << std::endl;
		tempServSockFDs.insert((*it)->getServSockFD());
	}

	return tempServSockFDs;
}

// Just a test response that directly sends to the client.
void TEST_RESPONSE(Client *tmpClient, int code, const std::string &message, const std::string &path)
{
	std::cout << "SENT" << std::endl;
	HttpResponse response(code, message);
	std::ifstream file(path.c_str());
	std::ostringstream body;
	body << file.rdbuf();
	response.setBody(body.str());
	std::vector<std::pair<std::string, std::string> > tmp;
	tmp.push_back(std::make_pair<std::string, std::string>("Content-Length", Tools::intToString(body.str().size())));
	response.setResponseHeaders(tmp);
	send(tmpClient->getFD(), response.getFinalResponse().c_str(), response.getFinalResponse().size(), MSG_NOSIGNAL);
}

const std::string& ServerManager::findPort(int eventFD) {
	std::vector<ServerSocket *>::const_iterator it = _serverSocketArray.begin();
	for (; it != _serverSocketArray.end(); it++) {
		if (eventFD == (*it)->getServSockFD())
			return (*it)->getPort();
	}
	return _serverSocketArray[0]->getPort();
}

std::pair<std::string, std::string> splitHostPair(const std::string& str) {
	std::vector<std::string> split = Tools::splitString(str, ":");
	return std::make_pair(split[0], split[1]);
}

Server* ServerManager::findServer(const std::string& host) {
	std::pair<std::string, std::string> hostPair = splitHostPair(host);

	int targetPort = std::atoi(hostPair.second.c_str());
	std::pair<int, std::string> exactKey(targetPort, hostPair.first);

	std::map<std::pair<int, std::string>, Server*>::const_iterator it = _serversMap.find(exactKey);
	if (it != _serversMap.end())  {// found exact match for Port + Server Name
		std::cout << LIGHT_BLUE << "Found exact match for " << exactKey.first
		<< ":" << exactKey.second << RESET << std::endl;
		return it->second;
	}

	std::pair<int, std::string> defaultKey(targetPort, "");
	it = _serversMap.lower_bound(defaultKey); // Find the first match for targetPort regardless of the Server Name
	if (it != _serversMap.end()) {
		std::cout << LIGHT_BLUE << "Found match for default port " << RESET << it->first.first
		<< " with server name: " << it->first.second << std::endl;
		return it->second;
	}

	throw Tools::Exception(500, "Error finding server");
}

bool locationMatchesPath(std::string& path, const std::string& location) {
	if (path == location)
		return true;

	// check if a path with a trailing / matches a location path
	if (!path.empty() && path.size() - 1 == location.size() && Tools::getLastCharacter(path) == '/')
		return path.compare(0, location.size(), location) == 0;
	
	return false;
}

const ConfigBase& findConfigBase(Server* server, const HttpRequest& request) {
	std::string path = request.getPath();
	std::cout << LIGHT_BLUE << "Path to look for is: " << path << RESET << std::endl;
	while (!path.empty()) {
		std::map<std::string, LocationConfig>::const_iterator it = server->getLocationConfigs().begin();
		for (; it != server->getLocationConfigs().end(); it++) {
			if (locationMatchesPath(path, it->first)) {
				std::cout << LIGHT_BLUE << "Found a match for the location: " << it->first << RESET << std::endl;
				return it->second;
			}
		}
		if (Tools::getLastCharacter(path) == '/')
			Tools::removeLastCharacter(path);
		
		Tools::eraseAfterLastCharacter(path, '/');
	}
	return *server;
}

// void handleReturnAndAllowMethod(const ConfigBase& base) {

// }

void ServerManager::checkRequestValidity(const Client &client, const HttpRequest &request, int eventFD) {
	std::string port = findPort(eventFD);
	(void)client;
	std::map<std::string, std::string>::const_iterator it = request.getHeader().find("Host");
	if (it == request.getHeader().end()) {
		throw Tools::Exception(400, "Host header missing");
	}
	
	Server *server = findServer(it->second);
	const ConfigBase& base = findConfigBase(server, request);
	// handleReturnAndAllowMethod(base);
	(void)base;
	(void)server;
}

void ServerManager::existingClient(unsigned int i, int eventFD)
{

	Client *tmpClient = _polling->handleExistingClient(eventFD, _polling->getEventArray()->events);
	(void)i;

	if (tmpClient)
	{

		TEST_RESPONSE(tmpClient, 200, "actually", "files/ascii/dog.html");

		HttpRequest request;
		if (request.parse("nonsense")) {
			tmpClient->setDoneReceiving(true);
		}
		
		if (request.getHeadersParsed() == true) {
			checkRequestValidity(*tmpClient, request, eventFD);
		}

		if (tmpClient->doneReceiving())
		{
			// Main logic:
			// 1. HttpRequest
			// 2. HttpMethod
			// 		responseToBeSent(true)

//			checkRequestValidity(*tmpClient, eventFD);

			if (tmpClient->responseToBeSent() && !tmpClient->readyToReceive())
			{
				// Set the EPOLLOUT event to be monitored.
				_polling->setClientEPOLLOUT(tmpClient, true);
			}
			if (tmpClient->readyToReceive() && tmpClient->responseToBeSent())
			{
				// 3. HttpResponse
			}
			if (tmpClient->responseSent())
			{
				// Remove the EPOLLOUT event
				_polling->setClientEPOLLOUT(tmpClient, false);
				tmpClient->refreshFlags();
			}
		}
		if (tmpClient->toBeClosed())
		{
			if (!_polling->deleteCLient(tmpClient))
				throw Tools::Exception("Error at deleting client");
		}
	}
	else
	{
		// Keep going boi
	}
}

bool ServerManager::matchServerFD(int eventFD) const
{
	if (_servSockFDs.find(eventFD) != _servSockFDs.end())
	{
		std::cout << ORANGE << "matchServerFD new client found from FD " << eventFD << RESET << std::endl;
		return true;
	}
	return false;
}

void ServerManager::eventLoop()
{
	while (!_sigStop)
	{
		_polling->epollWaitEvent();
		if (_polling->getEventCount() == -1)
		{
			if (errno == EINTR)
				return;
		}
		const epoll_event *eventArray = _polling->getEventArray();
		for (int i = 0; i < _polling->getEventCount(); i++)
		{
			int eventFD = eventArray[i].data.fd;
			_polling->setCurrEventFD(eventFD);
			if (matchServerFD(eventFD))
				_polling->registerNewClient(eventFD);
			else
				existingClient(i, eventFD);
		}
	}
}

void ServerManager::mainLoop()
{
	while (!_sigStop)
	{
		try
		{
			eventLoop();
		}
		catch (Tools::Exception &e)
		{
			if (e.getReturnCode() == 0)
				std::clog << GREEN << e.getMsgLog() << RESET << std::endl;
		}
		catch (std::exception &e)
		{
			std::clog << ORANGE << e.what() << RESET << std::endl;
		}
		catch (...)
		{
			std::clog << ORANGE << "Undefined error" << RESET << std::endl;
		}
	}
}
