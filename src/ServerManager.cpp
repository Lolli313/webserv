
#include "ServerManager.hpp"
#include "HttpMethod.hpp"

std::vector<Server *> setupServers(const std::vector<ServerBlockConfig> &serverConfigs);

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ServerManager::~ServerManager()
{
	std::clog << RED << "Calling ServerManager's destructor" << RESET << std::endl;
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
			std::clog << "ALREADY EXISTING SOCKET" << std::endl;
			_serverArray.push_back(new Server(*mit, *it));
		}
		found = false;
		// std::clog << CYAN_BRIGHT << "setupServers for fd = " << mit->getServSockFD() << RESET << std::endl;
	}
}

std::set<int> ServerManager::setupServSockFDs()
{
	std::set<int> tempServSockFDs;
	std::vector<Server *>::const_iterator it = _serverArray.begin();
	for (; it != _serverArray.end(); it++)
	{
		std::clog << YELLOW_BRIGHT << "setupServSockFDs for fd = " << (*it)->getServSockFD() << RESET << std::endl;
		tempServSockFDs.insert((*it)->getServSockFD());
	}

	return tempServSockFDs;
}

// Just a test response that directly sends to the client.
void TEST_RESPONSE(Client *tmpClient, int code, const std::string &message, const std::string &path)
{
	std::clog << "SENT" << std::endl;
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

void ServerManager::sendResponse(Client *client)
{
	int sent = send(client->getFD(), client->getResponseBuff().c_str() + client->getBytesSent(), client->getResponseBuff().size() - client->getBytesSent(), MSG_NOSIGNAL);

	if (sent < 0)
		throw Tools::Exception("sendResponse = -1");
	client->addBytesSent(sent);
	if (client->getBytesSent() >= client->getBuffer().size())
	{
		client->setResponseSent(true);
	}
}

// const std::string execute(const HttpRequest &request)
// {
// 	std::string response;
// 	if (_methodStr == "GET") {
// 		response = // GET // std::cout << "code pour get" << std::endl;
// 	} else if (_methodStr == "POST") {
// 		Post post(*this);
// 		post.parseBody();
// 		post.saveInFile();
// 		response = post.getResponse();
// 	} else if (_methodStr == "DELETE") {
// 		int fd = open(_path.c_str(), O_RDONLY);
// 		if (fd == -1) {
// 			throw Tools::Exception(500, "existe pas ou pas accessible");
// 		} else {
// 			std::remove(_path.c_str());
// 		}
// 		close(fd);
// 	}
// }

void ServerManager::existingClient(int eventFD)
{
	Client *tmpClient = _polling->handleExistingClient(eventFD, _polling->getEventArray()->events);
	if (tmpClient)
	{
		// TEST_RESPONSE(tmpClient, 200, "actually", "files/ascii/dog.html");

		// pour voir avant apres le buffer manager, il isole les request et set a true le done receiving
		// std::cout << RED << tmpClient->getBuffer() << RESET << std::endl;
		std::string tmpRequest = tmpClient->bufferManager();
		// std::cout << GREEN << tmpClient->getBuffer() << RESET << std::endl;

		if (tmpClient->doneReceiving())
		{
			HttpRequest request;
			request.parse(tmpRequest);
			// fait fonction
			std::map<std::string, std::string>::const_iterator itCookie = request.getHeader().find("Cookie");
			if (itCookie != request.getHeader().end()) {
				_cookie.setCookie(itCookie->second);
				// _cookie.printCookie();
			}
			// request.print();
			request.execute();
			// execute(request);
			// tmpClient->setResponseBuff(
			
			// Main logic:
			// 1. HttpRequest
			// 2. HttpMethod
			// 		responseToBeSent(true)
			if (tmpClient->responseToBeSent() && !tmpClient->readyToReceive())
			{
				// Set the EPOLLOUT event to be monitored.
				_polling->setClientEPOLLOUT(tmpClient, true);
			}
			if (tmpClient->readyToReceive() && tmpClient->responseToBeSent())
			{
				sendResponse(tmpClient);
			}
			if (tmpClient->responseSent())
			{
				// Remove the EPOLLOUT event
				_polling->setClientEPOLLOUT(tmpClient, false);
				tmpClient->refreshClient();
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
		std::clog << ORANGE << "matchServerFD new client found from FD " << eventFD << RESET << std::endl;
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
				existingClient(eventFD);
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
			// if (e.getReturnCode() == 0)
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
