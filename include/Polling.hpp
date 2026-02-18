
#ifndef POLLING_CLASS_HPP
#define POLLING_CLASS_HPP

#include "ServerSocket.hpp"
#include <netinet/in.h>
#include <sys/epoll.h>
#include "Client.hpp"
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <fcntl.h>
#include <utility>
#include <vector>
#include <map>

#define MAX_EVENTS 5
#define TIMEOUT -1
#define BUFFERSIZE 4096

class Polling
{
private:
	epoll_event _eventArray[MAX_EVENTS];
	std::map<const unsigned int, Client> _clientMap;
	std::vector<int> _servSockFDs;
	int _eventCount;
	int _epollFD;
	int _currEventFD;
	const int _newClientFlags;

	Polling();

public:
	Polling(const std::vector<ServerSocket>& servSockets);
	Polling(const Polling &obj);
	Polling &operator=(const Polling &obj);
	~Polling();

	int getEpollFD() const;
	void setCurrEventFD(int fd);
	int getCurrEventFD() const;
	int getServSockFD(int i) const;
	int getEventCount() const;
	int getNewClientFlags() const;
	Client &getClient(const unsigned int fd);

	const epoll_event *getEventArray() const;

	std::vector<int> setupAddServSockFDs(const std::vector<ServerSocket>& servSockets);

	void epollWaitEvent();
	void createEpoll();
	void addFDtoEpollAndClientMap(int targetFD, int eventFlags);
	
	void addFdToEpoll(int targetFD, int eventFlags);
	void addClientToEpoll(Client &client);
	bool deleteCLient(Client &client);
	void registerNewClient(int eventFD);
	void handleExistingClient(int eventFD, uint32_t currEvent);
	void handleClientInput(Client &client);

	void epollLoop();
	void runEventLoop();
	void successfulNewSocket(int newSocket);
	void failedNewSocket();
};

#endif
