
#ifndef POLLING_HPP
#define POLLING_HPP

#include "ServerSocket.hpp"
#include <netinet/in.h>
#include <algorithm>
#include <sys/epoll.h>
#include "Client.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <utility>
#include <vector>
#include <map>
#include <set>

#define MAX_EVENTS 5
#define TIMEOUT -1
#define BUFFERSIZE 4096

class Polling
{
private:
	epoll_event _eventArray[MAX_EVENTS];
	std::map<const unsigned int, Client> _clientMap;
	int _eventCount;
	int _epollFD;
	int _currEventFD;
	const int _newClientFlags;

	Polling();
	Polling &operator=(const Polling &obj);

public:
	Polling(const std::set<int>& servSockFDs);
	Polling(const Polling &obj);
	~Polling();

	int getEpollFD() const;
	void setCurrEventFD(int fd);
	int getCurrEventFD() const;
	int getEventCount() const;
	int getNewClientFlags() const;
	Client &getClient(const unsigned int fd);

	const epoll_event *getEventArray() const;

	std::vector<int> *setupAddServSockFDs(const std::vector<ServerSocket*>& servSockets);

	void epollWaitEvent();
	void createEpoll();
	void addFDtoEpollAndClientMap(int targetFD, int eventFlags);
	
	void addFdToEpoll(int targetFD, int eventFlags);
	void addClientToEpoll(Client &client);
	bool deleteCLient(Client *client);
	void registerNewClient(int eventFD);
	Client *handleExistingClient(int eventFD, uint32_t currEvent);
	void handleClientInput(Client &client);

	void successfulNewSocket(int newSocket);
	void failedNewSocket();


};

#endif
