#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include <string>
#include <unistd.h>

class Client
{
private:
	int _clientFD;
	std::string _buffer;
	bool _doneReceiving;

public:
	Client(int fd);
	Client(const Client &obj);
	Client &operator=(const Client &obj);
	~Client();

	int getFD();
	std::string &getBuffer();
	char *getBufferEnd();
	void setReceivingStatus(bool status);
	bool doneReceiving();
};

#endif
