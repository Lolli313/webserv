
#include "ServerManager.hpp"

void freeServSocket(ServerSocket* tmp);

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ServerManager::~ServerManager() {
	std::cout << "Calling ServerManager's destructor" << std::endl;
	std::for_each(_servSockets.begin(), _servSockets.end(), &freeServSocket);
}

ServerManager::ServerManager(std::vector<std::string> ports) :
_servSockets(setupServSockets(ports)),
_polling(_servSockets) {
	
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

void freeServSocket(ServerSocket* tmp) {
	delete tmp;
}

std::vector<ServerSocket*> setupServSockets(std::vector<std::string> ports) {
	std::vector<ServerSocket*> tempVector;
	for (std::size_t i = 0; i < ports.size(); i++) {
		ServerSocket *tempServSocket = new ServerSocket(ports[i]);
		tempVector.push_back(tempServSocket);
	}
	return tempVector;
}

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

bool ServerManager::matchServerFD(int eventFD) const {
	for (std::size_t i = 0; i < _servSockets.size(); i++)
	{
		std::cout << "match server fd" << std::endl;
		if (eventFD == _servSockets[i]->getServSockFD())
		{
			std::cout << ORANGE << "matchServerFD new client found" << RESET << std::endl;
			return true;
		}
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
				return ;
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
		try {
			eventLoop();
		}
		catch (Tools::Exception &e) {
			if (e.getReturnCode() == 0)
				std::clog << GREEN << e.getMsgLog() << RESET << std::endl;
		}
		catch (std::exception &e) {
			std::clog << ORANGE << e.what() << RESET << std::endl;
		}
		catch (...) {
			std::clog << RED << "Undefined error" << RESET << std::endl;
		}
	}
}

