
#include "ServerManager.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ServerManager::~ServerManager()
{
	std::cout << "Calling ServerManager's destructor" << std::endl;
}

ServerManager::ServerManager(std::vector<std::string> ports) : _servSockets(setupServSockets(ports)),
															   _polling(_servSockets)
{
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

void ServerManager::existingClient(unsigned int i, int eventFD)
{
	_polling.handleExistingClient(eventFD, _polling.getNewClientFlags());
	(void)i;
	// try
	// {
	// 	// // Client client = _polling.getClient(i);
	// 	// if (client.doneReceiving())
	// 	{
	// 		// HttpRequestParsing
	// 	}
	// }
	// catch (Tools::Exception &e)
	// {

	// }
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
	std::clog << YELLOW << "bonjour" << RESET << std::endl;
	while (!_sigStop)
	{
		std::clog << YELLOW << "ca va ?" << RESET << std::endl;
		_polling.epollWaitEvent();
		if (_polling.getEventCount() == -1)
		{
			if (errno == EINTR)
				return;
		}
		std::clog << YELLOW << "bah ecoute pas trop mal" << RESET << std::endl;
		const epoll_event *eventArray = _polling.getEventArray();

		std::clog << YELLOW << "NONNNNN" << RESET << std::endl;
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
			std::clog << RED << "Undefined error" << RESET << std::endl;
		}
	}
}
