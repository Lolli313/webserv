

#ifndef POLLING_CLASS_HPP
#define POLLING_CLASS_HPP

#include <sys/epoll.h>
#include <utility>
#include <map>
#include "Client.hpp"

class Polling
{
private:
	int _epollFD;
	std::map<int, Client> _clientMap;

	Polling();

public:
	Polling(const int servSockFD);
	Polling(const Polling &obj);
	Polling &operator=(const Polling &obj);
	~Polling();

	int getEpollFD() const;

	void createEpoll();
	void addFDtoEpoll(int targetFD);
	void epollLoop();
};

#endif
