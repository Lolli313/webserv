#include "Server.hpp"

void freeServSocket(ServerSocket* tmp);

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Server::Server(std::vector<std::string> ports) : 
	_servSockets(setupServSockets(ports)), 
	_polling(_servSockets) {}

Server::~Server() {
	std::for_each(_servSockets.begin(), _servSockets.end(), freeServSocket);
};

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

// Loop through the ports and call ServerSocket(ports[i])

void freeServSocket(ServerSocket* tmp) {
	delete tmp;
}

std::vector<ServerSocket*> Server::setupServSockets(std::vector<std::string> ports)
{
	std::vector<ServerSocket*> tempVector;
	for (std::size_t i = 0; i < ports.size(); i++)
	{
		ServerSocket *tempServSocket = new ServerSocket(ports[i]);
		tempVector.push_back(tempServSocket);
	}
	return tempVector;
}

void Server::existingClient(unsigned int i, int eventFD)
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

bool Server::matchServerFD(int eventFD) const
{
	for (std::size_t i = 0; i < _servSockets.size(); i++)
	{
		std::cout << "match server fd" << std::endl;
		if (eventFD == _servSockets[i]->getServSockFD())
			return true;
	}
	return false;
}

void Server::eventLoop()
{
	std::clog << YELLOW << "bonjour" << RESET << std::endl;
	while (1)
	{
		std::clog << YELLOW << "ca va ?" << RESET << std::endl;
		_polling.epollWaitEvent();
		std::clog << YELLOW << "bah ecoute pas trop mal" << RESET << std::endl;
		const epoll_event *eventArray = _polling.getEventArray();

		std::clog << YELLOW << "NONNNNN" << RESET << std::endl;
		for (int i = 0; i < _polling.getEventCount(); i++)
		{
			int eventFD = eventArray[i].data.fd;
			_polling.setCurrEventFD(eventArray[i].events);

			if (matchServerFD(eventFD))
				_polling.registerNewClient(eventFD);
			else
				existingClient(i, eventFD);
		}
	}
}

// Main loop that catches the exceptions and react accordingly
void Server::mainLoop()
{
	bool running = true;
	while (running)

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