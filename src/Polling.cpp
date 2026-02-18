
#include "terminalColors.hpp"
#include "Polling.hpp"
#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Polling::Polling(const std::vector<ServerSocket>& servSockets) :
	_servSockFDs(setupAddServSockFDs(servSockets)),
	_newClientFlags(EPOLLIN | EPOLLRDHUP | EPOLLERR)
{
	createEpoll();
	for (std::size_t i = 0; i < servSockets.size(); i++)
		addFDtoEpollAndClientMap(servSockets[i].getServSockFD(), _newClientFlags);
}

Polling::~Polling() {};

Polling::Polling(const Polling &obj) : _newClientFlags(obj._newClientFlags) { *this = obj; };

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

// int Polling::getServSockFD(int i) const { return _servSockFDs[i]; }

int Polling::getEventCount() const { return _eventCount; }

const epoll_event *Polling::getEventArray() const { return _eventArray; }

int Polling::getNewClientFlags() const { return _newClientFlags; }

// Exception on failure
Client &Polling::getClient(const unsigned int fd) { 
	std::map<const unsigned int, Client>::iterator itClient = _clientMap.find(fd);	
	if (itClient == _clientMap.end())
		Tools::Exception("Client not found");
	return itClient->second; 

}
/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

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
		throw Tools::Exception("epollEventAction");
}

std::vector<int> Polling::setupAddServSockFDs(const std::vector<ServerSocket>& servSockets) {
	std::vector<int> temp;
	for (std::size_t i = 0; i < servSockets.size(); i++) {
		temp.push_back(servSockets[i].getServSockFD());
	}
	return temp;
}

void Polling::addFdToEpoll(int targetFD, int eventFlags)
{
	epollEventAction(_epollFD, targetFD, EPOLL_CTL_ADD, eventFlags);
}

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

// Exception on failure
void Polling::createEpoll()
{
	_epollFD = epoll_create1(0);
	if (_epollFD < 0)
		throw Tools::Exception("createEpoll");
}

// Exception on failure
void Polling::successfulNewSocket(int newSocket) {
	fcntl(newSocket, F_SETFL, O_NONBLOCK);
	addFDtoEpollAndClientMap(newSocket, _newClientFlags);
}

void Polling::failedNewSocket() {
	throw Tools::Exception("failedNewSocket");
}

// Exception on failure
void Polling::registerNewClient(int serverSocketFD) {
	int newSocket;
	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	newSocket = accept(serverSocketFD, (sockaddr *)&clientAddr, &clientLen);
	if (newSocket >= 0)
		successfulNewSocket(newSocket);
	else
		failedNewSocket();
}

// Exception on failure
void Polling::handleClientInput(Client &client)
{
	int readSize = recv(_currEventFD, client.getTmpBufferPtr(), BUFFERSIZE, 0);
	if (readSize < 0)
	{
		deleteCLient(client);
		throw Tools::Exception("error at receiving client input");
	}
	else if (readSize == 0)
	{
		client.getBuffer().append(client.getTmpBufferPtr(), readSize);
		client.setReceivingStatus(true);
	}
}

// Exception on failure
//
// Receives client input and client diconnection
void Polling::handleExistingClient(int clientFD, uint32_t currEvent) {
	std::cout << "Found an exising connection" << std::endl;

	std::map<const unsigned int, Client>::iterator itClient = _clientMap.find(clientFD);
	
	if (itClient == _clientMap.end())
		throw Tools::Exception("Client not found");

	// CLIENT DISCONNECTED
	if (currEvent & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
	{
		if (!deleteCLient(itClient->second))
			throw Tools::Exception("Error at deleting client");
	}
	// CLIENT INPUT
	else if (currEvent & EPOLLIN)
	{
		handleClientInput(itClient->second);
	}
}

void Polling::epollWaitEvent()
{
	_eventCount = epoll_wait(_epollFD, _eventArray, MAX_EVENTS, TIMEOUT);
}
