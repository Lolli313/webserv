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

	// std::size_t bytesSent; // Already sent bytes, an index for _buffer, waiting for client to send be ready to receive.

	bool _doneReceiving; // Once the message is fully received
	// Depending on the request, soemtimes no response should be sent.
	// Base value = 0;
	// if value < 1, do not send a response
	int _responseToBeSent;
	bool _responseSent; // The response has been sent to this client, if _keepAlive, should be reset (i guess)
	bool _keepAlive;
	bool _readyToReceive; // Client is waiting for response
	bool _toBeClosed; // CLient should be closed, can be set by EPOLLHUP

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

	bool responseToBeSent() const;
	void setResponseToBeSent(int status);	

	bool readyToReceive() const;
	void setReadyToReceive(bool status);	

	bool responseSent() const;
	void setResponseSent(bool status);	

	void setToBeClosed(bool status);
	bool toBeClosed() const;
};

#endif
