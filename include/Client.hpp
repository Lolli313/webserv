#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include <string>
#include <unistd.h>

#define BUFFERSIZE 4096

class Client
{
private:
	const int _clientFD;
	std::string _buffer;
	char _tmpBuff[BUFFERSIZE];
	bool _doneReceiving;
	// HttpRequest class

	Client &operator=(const Client &obj);

public:
	Client(int fd);
	Client(const Client &obj);
	~Client();

	int getFD();
	std::string &getBuffer();
	char *getTmpBufferPtr();
	// std::string &getTmpBuffer();
	void setReceivingStatus(bool status);
	bool doneReceiving() const;
};

#endif
