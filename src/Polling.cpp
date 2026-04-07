
#include "TerminalColors.hpp"
#include "Polling.hpp"
#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Polling::Polling(const std::set<int> &servSockFDs) : 
	_newClientFlags(EPOLLIN | EPOLLRDHUP | EPOLLERR)
{
	createEpoll();
	LOG(INFO, PURPLE, "epoll CONSTRUCTOR, socket size is", Tools::intToString(servSockFDs.size()));
	for (std::set<int>::iterator it = servSockFDs.begin(); it != servSockFDs.end(); it++)
	{
		addFdToEpoll(*it, _newClientFlags);
	}
}

Polling::~Polling()
{
	LOG(INFO, RED_BRIGHT, "Calling Polling destructor");
	for (std::map<const unsigned int, Client *>::iterator it = _clientMap.begin(); it != _clientMap.end();)
	{
		std::map<const unsigned int, Client *>::iterator curr = it++;
		deleteCLient(curr->second);
	}
	Tools::closeAndResetFD(_epollFD);
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
	LOG(INFO, GREEN, "Adding to epoll fd", Tools::intToString(targetFD));
	epollEventAction(_epollFD, targetFD, EPOLL_CTL_ADD, eventFlags);
}

void Polling::deleteFdFromEpoll(int targetFD) {
	LOG(INFO, GREEN, "deleting fd from epoll", Tools::intToString(targetFD));
	// epollEventAction(_epollFD, targetFD, EPOLL_CTL_DEL, 0);
	epoll_ctl(_epollFD, EPOLL_CTL_DEL, targetFD, NULL);
}

// Exception on failure
void Polling::addFDtoEpollAndClientMap(int targetFD, int eventFlags, sockaddr_in& clientAddr)
{
	epollEventAction(_epollFD, targetFD, EPOLL_CTL_ADD, eventFlags);
	Client *client = new Client(targetFD, clientAddr);
	_clientMap[targetFD] = client;
	_clientVector.push_back(client);
	LOG(INFO, GREEN, "Adding FD to epoll and client maps");
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
	LOG(INFO, BLUE, "DELETE CLIENT");
	epollEventAction(_epollFD, client->getFD(), EPOLL_CTL_DEL, 0);
	if ((_clientMap.erase(client->getFD())) != 1)
		return (false);
	Tools::closeAndResetFD(client->getRefFD());
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
void Polling::successfulNewSocket(int newSocket, sockaddr_in& clientAddr)
{
	LOG(INFO, PINK, "Succesfully created new socket for client");
	fcntl(newSocket, F_SETFL, O_NONBLOCK);
	addFDtoEpollAndClientMap(newSocket, _newClientFlags, clientAddr);
}

void Polling::failedNewSocket()
{
	throw Tools::Exception("failedNewSocket");
}

// Exception on failure
void Polling::registerNewClient(int serverSocketFD)
{
	LOG(INFO, LIGHT_BLUE, "Registering a new client n" + Tools::intToString(_clientMap.size()));
	int newSocket;
	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	newSocket = accept(serverSocketFD, (sockaddr *)&clientAddr, &clientLen);
	if (newSocket >= 0)
		successfulNewSocket(newSocket, clientAddr);
	else
		failedNewSocket();
}

// Exception on failure
void Polling::readClientInput(Client &client)
{
	LOG(INFO, GREEN_BRIGHT, "HandleClientInput for fd", Tools::intToString(_currEventFD));
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
		LOG(INFO, MAGENTA, "EOF");
	}
}

/**
 * @brief Receives client input and client diconnection
 * @exception Throws on failure
 **/
Client *Polling::handleExistingClient(int clientFD, uint32_t currEvent)
{
	LOG(INFO, LIME, "Found an existing connection");

	if (_clientMap.find(clientFD) == _clientMap.end())
	{
		LOG(INFO, "found a CGI pipe fd " + Tools::intToString(clientFD));
		return NULL;
	}
	LOG(INFO, ORANGE, "Found clientFD match for FD", Tools::intToString(clientFD));

	std::map<const unsigned int, Client *>::iterator itClient = _clientMap.find(clientFD);

	if (itClient == _clientMap.end())
		throw Tools::Exception("Client not found");

	// ERROR
	if (currEvent & EPOLLERR)
	{
		LOG(ERROR, "EPOLLERR");
		int error = 0;
		socklen_t len = sizeof(error);
		if (getsockopt(clientFD, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
			LOG(ERROR, "getsockopt error");
		}
		if (error != 0) {
			LOG(ERROR, Logger::getLevelColor(ERROR), "Socket error", strerror(error));
		}
		itClient->second->setToBeClosed(true);
	}

	// CLIENT DISCONNECTED
	if (currEvent & EPOLLHUP)
	{
		LOG(INFO, "EPOLLHUP");
		readClientInput(*itClient->second);
		itClient->second->setDoneReceiving(true);
		itClient->second->setToBeClosed(true);
		itClient->second->setResponseToBeSent(-1); // No response should be sent
	}

	// CLIENT IS DONE SENDING
	// We should set _doneReceiving = true (i guess)
	if (currEvent & EPOLLRDHUP)
	{
		LOG(INFO, "EPOLLRDHUP");
		itClient->second->setDoneReceiving(true);
		itClient->second->setToBeClosed(true);
	}

	// CLIENT INPUT
	if (currEvent & EPOLLIN)
	{
		LOG(INFO, "EPOLLIN");
		readClientInput(*itClient->second);
	}

	// CLIENT READY TO RECEIVE
	if (currEvent & EPOLLOUT)
	{
		LOG(INFO, PINK, "EPOLLOUT");
		itClient->second->setReadyToReceive(true);
	}
	return itClient->second;
}

void Polling::epollWaitEvent()
{
	// LOG(INFO, DEFAULT, "epoll WAITING, " + Tools::intToString(_clientMap.size()) + " clients");
	_eventCount = epoll_wait(_epollFD, _eventArray, MAX_EVENTS, TIMEOUT);
}