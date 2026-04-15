
#ifndef POLLING_HPP
#define POLLING_HPP

#include "ServerSocket.hpp"
#include "HttpResponse.hpp"
#include "Client.hpp"
#include "Tools.hpp"
#include "CGI.hpp"

#include <netinet/in.h>
#include <sys/epoll.h>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <utility>
#include <vector>
#include <map>
#include <set>

#define TIMEOUT 60
#define EPOLL_TIMEOUT 1000

class Polling
{
private:
	epoll_event _eventArray[MAX_EVENTS];
	std::map<const unsigned int, Client *> _clientMap;
	std::vector<Client *> _clientVector; // Used to retrieve the timestamp of each client, FD is used to get a O(1) loolkup
	std::map<CGI*, Client*> _CGImap;
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
	Client *getClientPtr(const unsigned int fd);
	std::vector<Client *> &getClientVector();
	std::map<CGI*, Client*> &getCgiMap();

	const epoll_event *getEventArray() const;

	std::vector<int> *setupAddServSockFDs(const std::vector<ServerSocket*>& servSockets);

	void epollWaitEvent();
	void createEpoll();
	void addFDtoEpollAndClientMap(int targetFD, int eventFlags, sockaddr_in& clientAddr);
	
	void addFdToEpoll(int targetFD, int eventFlags);
	void addClientToEpoll(Client &client);
	void deleteFdFromEpoll(int targetFD);
	bool deleteClient(Client *client);
	void registerNewClient(int eventFD);
	Client *handleClientEvent(Client *client, uint32_t currEvent);
	void readClientInput(Client &client);

	void successfulNewSocket(int newSocket, sockaddr_in& clientAddr);
	void failedNewSocket();

	void setClientEPOLLOUT(Client *client, bool add);
};

#endif
