
#ifndef POLLING_CLASS_HPP
#define POLLING_CLASS_HPP

#include <sys/epoll.h>
#include "Client.hpp"
#include <iostream>
#include <utility>
#include <map>

#define MAX_EVENTS 5

// TODO add servSockFDs to this class

class Polling
{
private:
	int _epollFD;
	std::map<int, Client> _clientMap;
	int _eventCount;
	struct epoll_event _eventArray[MAX_EVENTS];

	Polling();

public:
	Polling(const int servSockFD);
	Polling(const Polling &obj);
	Polling &operator=(const Polling &obj);
	~Polling();

	int getEpollFD() const;

	void createEpoll();
	void addFDtoEpoll(int targetFD);
	void addClientToEpoll(Client client);
	void epollLoop();
	void runEventLoop();
};

#endif
