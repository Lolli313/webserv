
#include "TerminalColors.hpp"
#include "Polling.hpp"
#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Polling::Polling(const std::set<int> &servSockFDs) : // _servSockFDs(setupAddServSockFDs(servSockets)),
													 // _servSockets(servSockets),
													 _newClientFlags(EPOLLIN | EPOLLRDHUP | EPOLLERR)
{
	createEpoll();
	std::clog << PURPLE << "epoll CONSTRUCTOR, socket seize is: " << servSockFDs.size() << RESET << std::endl;
	for (std::set<int>::iterator it = servSockFDs.begin(); it != servSockFDs.end(); it++)
	{
		addFdToEpoll(*it, _newClientFlags);
	}
}

Polling::~Polling()
{
	std::clog << RED << "Calling Polling destructor" << RESET << std::endl;
	for (std::map<const unsigned int, Client *>::iterator it = _clientMap.begin(); it != _clientMap.end();)
	{
		std::map<const unsigned int, Client *>::iterator curr = it++;
		deleteCLient(curr->second);
	}
	close(_epollFD);
}

Polling::Polling(const Polling &obj) : _newClientFlags(obj._newClientFlags) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
Polling &Polling::operator=(const Polling &obj)
{
	(void)obj;
	return (*this);
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Polling::getEpollFD() const { return _epollFD; }

void Polling::setCurrEventFD(int fd) { _currEventFD = fd; }

int Polling::getCurrEventFD() const { return _currEventFD; }

int Polling::getEventCount() const { return _eventCount; }

const epoll_event *Polling::getEventArray() const { return _eventArray; }

int Polling::getNewClientFlags() const { return _newClientFlags; }

// Exception on failure
Client &Polling::getClient(const unsigned int fd)
{
	std::map<const unsigned int, Client *>::iterator itClient = _clientMap.find(fd);
	if (itClient == _clientMap.end())
		Tools::Exception("Client not found");
	return *itClient->second;
}

std::vector<Client *> &Polling::getClientVector() { return _clientVector; }
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
	std::memset(&event, 0, sizeof(event));

	event.events = epollEventFlag;
	event.data.fd = targetFd;

	if (epoll_ctl(epollFD, epollEvent, targetFd, &event))
		throw Tools::Exception("epollEventAction");
}

/**
 * @brief Add or remove the EPOLLOUT of the client to the epoll.
 * @param client The target client.
 * @param add 1 to add and 0 to remove EPOLLOUT.
 * @exception Exception on failure.
 */
void Polling::setClientEPOLLOUT(Client *client, bool add)
{
	int mask = _newClientFlags;
	if (add)
		mask |= EPOLLOUT; 
	epollEventAction(_epollFD, client->getFD(), EPOLL_CTL_MOD, mask);
}

void Polling::addFdToEpoll(int targetFD, int eventFlags)
{
	std::clog << GREEN << "Adding fd " << targetFD << " to epoll." << RESET << std::endl;
	epollEventAction(_epollFD, targetFD, EPOLL_CTL_ADD, eventFlags);
}

// Exception on failure
void Polling::addFDtoEpollAndClientMap(int targetFD, int eventFlags)
{
	epollEventAction(_epollFD, targetFD, EPOLL_CTL_ADD, eventFlags);
	Client *client = new Client(targetFD);
	_clientMap[targetFD] = client;
	_clientVector.push_back(client);
	std::clog << "Adding FD to epoll and client maps" << std::endl;
}

// // Exception on failure
// // SHOULD WE USE REFERENCE OR NOT ?
// void Polling::addClientToEpoll(Client &client)
// {
// 	epollEventAction(_epollFD, client.getFD(), EPOLL_CTL_ADD, EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP | EPOLLOUT);
// 	_clientMap[client.getFD()] = new Client(client);
// 	_clientMap.insert(std::make_pair(client.getFD(), client));
// }

// returns true if client deleted, false on error
bool Polling::deleteCLient(Client *client)
{
	std::clog << BLUE << "DELETE CLIENT" << RESET << std::endl;
	epollEventAction(_epollFD, client->getFD(), EPOLL_CTL_DEL, 0);
	close(client->getFD());
	if ((_clientMap.erase(client->getFD())) != 1)
		return (false);
	for (std::vector<Client *>::iterator it = _clientVector.begin(); it != _clientVector.end(); it++)
	{
		if (*it == client)
		{
			_clientVector.erase(it);
			break;
		}
	}
	delete client;
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
void Polling::successfulNewSocket(int newSocket)
{
	std::clog << "Succesfully created new socket for client :)" << std::endl;
	fcntl(newSocket, F_SETFL, O_NONBLOCK);
	addFDtoEpollAndClientMap(newSocket, _newClientFlags);
}

void Polling::failedNewSocket()
{
	throw Tools::Exception("failedNewSocket");
}

// Exception on failure
void Polling::registerNewClient(int serverSocketFD)
{
	std::clog << LIGHT_BLUE << "Registering a new client n" << _clientMap.size() << RESET << std::endl;
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
void Polling::readClientInput(Client &client)
{
	std::clog << GREEN_BRIGHT << "HandleClientInput for fd = " << _currEventFD << RESET << std::endl;
	int readSize = recv(_currEventFD, client.getTmpBufferPtr(), BUFFERSIZE, 0);
	if (readSize < 0)
	{
		deleteCLient(&client);
		throw Tools::Exception("error at receiving client input");
	}
	else if (readSize > 0)
	{
		client.getBuffer().append(client.getTmpBufferPtr(), readSize);
	}
	else
	{
		// MAYBE CLOSE THE CONNECTION HERE
		client.setDoneReceiving(true);
		std::clog << MAGENTA << "EOF" << RESET << std::endl;
	}
}

/**
 * @brief Receives client input and client diconnection
 * @exception Throws on failure
 **/
Client *Polling::handleExistingClient(int clientFD, uint32_t currEvent)
{
	std::clog << "Found an existing connection" << std::endl;

	if (_clientMap.find(clientFD) == _clientMap.end())
	{
		std::clog << "Unexpected no match for existing client" << std::endl;
		return NULL;
	}
	else {}
		std::clog << ORANGE << "Found clientFD match for FD: " << clientFD << RESET << std::endl;

	std::map<const unsigned int, Client *>::iterator itClient = _clientMap.find(clientFD);

	if (itClient == _clientMap.end())
		throw Tools::Exception("Client not found");

	// ERROR
	if (currEvent & EPOLLERR)
	{
		std::clog << RED << "EPOLLERR" << RESET << std::endl;
		int error = 0;
		socklen_t len = sizeof(error);
		if (getsockopt(clientFD, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
			std::clog << RED << "getsockopt error" << RESET << std::endl;
		if (error != 0)
			std::clog << RED << "Socket error " << strerror(error) << RESET << std::endl;
		itClient->second->setToBeClosed(true);
	}

	// CLIENT DISCONNECTED
	if (currEvent & EPOLLHUP)
	{
		std::clog << "EPOLLHUP" << std::endl;
		readClientInput(*itClient->second);
		itClient->second->setDoneReceiving(true);
		itClient->second->setToBeClosed(true);
		itClient->second->setResponseToBeSent(-1); // No response should be sent
	}

	// CLIENT IS DONE SENDING
	// We should set _doneReceiving = true (i guess)
	if (currEvent & EPOLLRDHUP)
	{
		std::clog << "EPOLLRDHUP" << std::endl;
		itClient->second->setDoneReceiving(true);
		itClient->second->setToBeClosed(true);
	}

	// CLIENT INPUT
	if (currEvent & EPOLLIN)
	{
		
		std::clog << "EPOLLIN" << std::endl;
		readClientInput(*itClient->second);
	}

	// CLIENT READY TO RECEIVE
	if (currEvent & EPOLLOUT)
	{
		std::clog << PINK << "EPOLLOUT" << RESET << std::endl;
		itClient->second->setReadyToReceive(true);
	}
	return itClient->second;
}

void Polling::epollWaitEvent()
{
	// std::clog << "epoll WAITING, " << _clientMap.size() << " clients." << std::endl;
	_eventCount = epoll_wait(_epollFD, _eventArray, MAX_EVENTS, TIMEOUT);
}