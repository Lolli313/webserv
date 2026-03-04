
#include "ServerManager.hpp"
#include "HttpRequest.hpp"

std::vector<Server> setupServers(std::vector<std::string> ports);
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
}

ServerManager::ServerManager(const std::vector<std::string> &ports) : _serverArray(setupServers(ports)), _servSockFDs(setupServSockFDs()), _polling(_servSockFDs) {}

// ServerManager::ServerManager(ParseConfig pc) {

// }

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

std::vector<Server> setupServers(std::vector<std::string> ports)
{
	std::vector<Server> tempServers;
	for (std::size_t i = 0; i < ports.size(); i++)
	{
		tempServers.push_back(ports[i]);
		std::cout << CYAN_BRIGHT << "setupServers for fd = " << tempServers[i].getServSockFD() << RESET << std::endl;
	}
	return tempServers;
}

std::set<int> ServerManager::setupServSockFDs()
{
	std::set<int> tempServSockFDs;
	for (std::vector<Server>::iterator it = _serverArray.begin(); it != _serverArray.end(); it++)
	{
		std::cout << YELLOW_BRIGHT << "setupServSockFDs for fd = " << it->getServSockFD() << RESET << std::endl;
		tempServSockFDs.insert(it->getServSockFD());
	}

	return tempServSockFDs;
}

void ServerManager::existingClient(unsigned int i, int eventFD)
{

	Client *tmpClient = _polling.handleExistingClient(eventFD, _polling.getEventArray()->events);
	(void)i;

	if (tmpClient)
	{
		// Do something smart:
		// Call HttpRequest parser
		// Call the HttpMethod
		// Send the response
		// Then, delete the client
		// HttpRequest test;
		// test.parse(tmpClient->getTmpBufferPtr());
		if (!_polling.deleteCLient(tmpClient))
			throw Tools::Exception("Error at deleting client");
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
		std::cout << ORANGE << "matchServerFD new client found" << RESET << std::endl;
		return true;
	}
	return false;
}

void ServerManager::eventLoop()
{
	while (!_sigStop)
	{
		_polling.epollWaitEvent();
		if (_polling.getEventCount() == -1)
		{
			if (errno == EINTR)
				return;
		}
		const epoll_event *eventArray = _polling.getEventArray();
		for (int i = 0; i < _polling.getEventCount(); i++)
		{
			int eventFD = eventArray[i].data.fd;
			_polling.setCurrEventFD(eventFD);
			if (matchServerFD(eventFD))
				_polling.registerNewClient(eventFD);
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

