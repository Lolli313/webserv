
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
	std::cout << RED << "Calling ServerManager's destructor" << RESET << std::endl;
	for (std::set<int>::iterator it = _servSockFDs.begin(); it != _servSockFDs.end(); it++)
		close(*it);
	for (std::vector<Server *>::iterator it = _serverArray.begin(); it != _serverArray.end(); it++)
		delete (*it);
	delete _polling;
}

ServerManager::ServerManager(const std::vector<ServerBlockConfig> &serverConfigs)
{
	try
	{
	_serverArray = setupServers(serverConfigs);
	_serversMap = setupServersMap();
	_servSockFDs = setupServSockFDs();
	_polling = new Polling(_servSockFDs);
	}
	catch (Tools::Exception &e)
	{
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

std::map<std::pair<int, std::string>, Server*> ServerManager::setupServersMap()
{
	std::map<std::pair<int, std::string>, Server*> tmpMap;
	std::vector<Server *>::iterator it = _serverArray.begin();
	for (; it != _serverArray.end(); it++) {
		std::set<std::string>::const_iterator sit = (*it)->getServerNames().begin();
		for (;sit != (*it)->getServerNames().end(); sit++)
			tmpMap.insert(std::make_pair
				(std::make_pair(std::atoi((*it)->getPort().c_str()), *sit), (*it)));
	}
	return tmpMap;
}

std::vector<Server *> setupServers(const std::vector<ServerBlockConfig> &serverConfigs)
{
	bool found = false;
	std::vector<Server *> tempServers;
	for (std::vector<ServerBlockConfig>::const_iterator mit = serverConfigs.begin(); mit != serverConfigs.end(); mit++) {
		std::vector<Server *>::iterator it = tempServers.begin();
		for (;it != tempServers.end(); it++)
		{
			if ((*it)->getPort() == mit->getPort())
			{
				found = true;
				break;
			}
		}
		if (!found)
			tempServers.push_back(new Server(*mit));
		else
			tempServers.push_back(new Server(*mit, (*it)->getServSocket()));
		found = false;
		// std::cout << CYAN_BRIGHT << "setupServers for fd = " << mit->getServSockFD() << RESET << std::endl;
	}
	return tempServers;
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

void ServerManager::existingClient(unsigned int i, int eventFD)
{

	Client *tmpClient = _polling->handleExistingClient(eventFD, _polling->getEventArray()->events);
	(void)i;

	if (tmpClient)
	{

		TEST_RESPONSE(tmpClient, 200, "actually", "files/ascii/dog.html");

		if (tmpClient->doneReceiving())
		{
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
		std::cout << ORANGE << "matchServerFD new client found from FD " << eventFD <<  RESET << std::endl;
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
