
#ifndef POLLING_CLASS_HPP
#define POLLING_CLASS_HPP

#include <netinet/in.h>
#include <sys/epoll.h>
#include "Client.hpp"
#include <iostream>
#include <fcntl.h>
#include <utility>
#include <map>

#define MAX_EVENTS 5
#define TIMEOUT -1
#define BUFFERSIZE 4096

class Polling
{
private:
	struct epoll_event _eventArray[MAX_EVENTS];
	std::map<const unsigned int, Client> _clientMap;
	int _eventCount;
	int _epollFD;
	int _servSockFD;
	int _currEventFD;
	const int _newClientFlags;

	Polling();

public:
	Polling(const int servSockFD);
	Polling(const Polling &obj);
	Polling &operator=(const Polling &obj);
	~Polling();

	int getEpollFD() const;
	void setCurrEventFD(int fd);
	int getCurrEventFD() const;

	void createEpoll();
	void addFDtoEpollAndClientMap(int targetFD, int eventFlags);
	
	void addClientToEpoll(Client &client);
	bool deleteCLient(Client &client);
	void registerNewClient(int eventFD);
	void handleExistingClient(int eventFD, uint32_t currEvent);
	void handleClientInput(Client &client);

	void epollLoop();
	void runEventLoop();
	void successfulNewSocket(int eventFD, int newSocket);
	void failedNewSocket(int eventFD);
};

#endif
