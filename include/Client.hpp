#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <cstring>

#define BUFFERSIZE 4096

class Client
{
private:
	const int _clientFD;
	std::string _buffer;
	char _tmpBuff[BUFFERSIZE];

	bool _doneReceiving; // Once the message is fully received
	bool _responseSent; // The response has been sent to this client, if _keepAlive, should be reset (i guess)
	bool _keepAlive;

	Client &operator=(const Client &obj);
	
public:
	Client(int fd);
	Client(const Client &obj);
	~Client();

	int getFD();
	std::string &getBuffer();
	
	char *getTmpBufferPtr();
	
	bool isKeepAlive() const;
	void setKeepAlive(bool status);
	
	void setReceivingStatus(bool status);
	bool doneReceiving() const;

	bool responseSent() const;
	void setResponseSent(bool status);	

	bool toBeClosed() const;
};

#endif
