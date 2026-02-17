
#include "terminalColors.hpp"
#include "Polling.hpp"
#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Polling::Polling(const int servSockFD) : _servSockFD(servSockFD), _newClientFlags(EPOLLIN | EPOLLRDHUP | EPOLLERR)
{
	createEpoll();
	addFDtoEpollAndClientMap(servSockFD, _newClientFlags);
}

Polling::~Polling() {};

Polling::Polling(const Polling &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
Polling &Polling::operator=(const Polling &obj)
{
	if (this != &obj)
	{
	}
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Polling::getEpollFD() const { return _epollFD; }

void Polling::setCurrEventFD(int fd) { _currEventFD = fd; }

int Polling::getCurrEventFD() const { return _currEventFD; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

// Exception on failure
void Polling::addFDtoEpollAndClientMap(int targetFD, int eventFlags)
{
	epollEventAction(_epollFD, targetFD, EPOLL_CTL_ADD, eventFlags);
	_clientMap.insert(std::make_pair(targetFD, Client(targetFD)));
}

// Exception on failure
// SHOULD WE USE REFERENCE OR NOT ?
void Polling::addClientToEpoll(Client &client)
{
	epollEventAction(_epollFD, client.getFD(), EPOLL_CTL_ADD, EPOLLIN | EPOLLRDHUP | EPOLLERR);
	_clientMap.insert(std::make_pair(client.getFD(), client));
}

// returns true if client deleted, false on error
bool Polling::deleteCLient(Client &client)
{
	if ((_clientMap.erase(client.getFD())) != 1)
		return (false);
	return (true);
}

/**
 * Exception on failure
 *
 * Register , modify or delete a socket (targetFD) in the epoll intereset list.
 * Types of epoll_events:
 * - EPOLL_CTL_ADD
 * - EPOLL_CTL_MOD = to change the epollEventFlag.
 * - EPOLL_CTL_DEL
 */
void epollEventAction(int epollFD, int targetFd, int epollEvent, int epollEventFlag)
{
	struct epoll_event event;

	event.events = epollEventFlag;
	event.data.fd = targetFd;

	if (epoll_ctl(epollFD, epollEvent, targetFd, &event))
		throw Tools::Exception("epollEventAction for " + targetFd);
}

// Exception on failure
void Polling::createEpoll()
{
	_epollFD = epoll_create1(0);
	if (_epollFD < 0)
		throw Tools::Exception("createEpoll");
}

void Polling::successfulNewSocket(int serverSocketFD, int newSocket) {
	fcntl(newSocket, F_SETFL, O_NONBLOCK);
	addFDtoEpollAndClientMap(newSocket, _newClientFlags);
}

void failedNewSocket(int eventFD, int newSocket) {
	throw Tools::Exception("failedNewSocket");
}


void Polling::registerNewClient(int serverSocketFD) {
	int newSocket;
	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	newSocket = accept(serverSocketFD, (sockaddr *)&clientAddr, &clientLen);
	if (newSocket >= 0)
		successfulNewSocket(serverSocketFD, newSocket);
	else
		failedNewSocket(serverSocketFD);
}

void Polling::handleClientInput(Client &client)
{
	int readSize = recv(_currEventFD, client.getBufferEnd(), BUFFERSIZE, 0);
	if (readSize < 0)
	{
		deleteCLient(client);
		throw Tools::Exception("error at receiving client input");
	}
	else if (readSize == 0)
		client.setReceivingStatus(true);

}

// Exception on failure
void Polling::handleExistingClient(int clientFD, uint32_t currEvent) {
	std::cout << "Found an exising connection" << std::endl;

	std::map<const unsigned int, Client>::iterator itClient = _clientMap.find(clientFD);
	
	if (itClient == _clientMap.end())
		throw Tools::Exception("client not found");
	// CLIENT DISCONNECTED
	if (currEvent & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
	{
		if (!deleteCLient(itClient->second))
			throw Tools::Exception("error at deleting client fd " + clientFD);
	}
	// CLINT INPUT
	else if (currEvent & EPOLLIN)
	{
		handleClientInput(itClient->second);
	}

	if (itClient->second.doneReceiving())
	{
		// Handle end of reception of the client
	}
}

void Polling::runEventLoop()
{
	_eventCount = epoll_wait(_epollFD, _eventArray, MAX_EVENTS, TIMEOUT);
	std::cout << PURPLE << _eventCount << " events ready" << RESET << std::endl;

	// NEW EVENTS: GO THROUGH EACH EVENT
	for (int i = 0; i < _eventCount; i++)
	{
		int eventFD = _eventArray[i].data.fd;
		_currEventFD = _eventArray[i].events;

		// REGISTER NEW SOCKET / CLIENT
		if (eventFD == _servSockFD)
			registerNewClient(eventFD);
		else
			handleExistingClient(eventFD, _currEventFD);
	}
}

void Polling::epollLoop()
{
	bool running = true;
	while (running)
	{
		runEventLoop();
		try
		{
			runEventLoop();
		}
		catch (Tools::Exception &e)
		{
			if (e.returnCode == 0)
				std::clog <<
		}
		catch (std::exception &e)
		{
			// unexpected exception.
		}
		catch (...)
		{

		}
	}
}
