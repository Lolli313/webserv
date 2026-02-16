
#include "Polling.hpp"
#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Polling::Polling(const int servSockFD)
{
	createEpoll();
	addFDtoEpoll(servSockFD);
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

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

// Exception on failure
void Polling::addFDtoEpoll(int targetFD)
{
	epollEventAction(_epollFD, targetFD, EPOLL_CTL_ADD, EPOLLIN | EPOLLRDHUP | EPOLLERR);
	_clientMap.insert(std::make_pair(targetFD, Client(targetFD)));
}

// Exception on failure
void Polling::addClientToEpoll(Client client)
{
	epollEventAction(_epollFD, client.getFD(), EPOLL_CTL_ADD, EPOLLIN | EPOLLRDHUP | EPOLLERR);
	_clientMap.insert(std::make_pair(client.getFD(), client));
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

void epollLoop()
{
	struct epoll_event eventArray[MAX_EVENTS];
	int eventCount;
	bool running = true;
	while (running)
	{
	}
	try catch (Tools::Exception &e)
	{
		if (
		if (e.returnCode == 0)
			std::clog <<
	}
	catch (std::exception &e)
	{
		// unexpected exception.
	}
}
