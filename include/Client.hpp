#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <ctime>

#define BUFFERSIZE 4096

class Client
{
private:

	int _clientFD;
	std::string _buffer; // Receives the input
	char _tmpBuff[BUFFERSIZE];
	sockaddr_in _clientAddr;

	std::string _responseBuff; // The response buffer
	std::size_t _bytesSent; // Already sent bytes, an index for _responseBuff, waiting for client to send be ready to receive.

	bool _doneReceiving; // Once the message is fully received
	// Depending on the request, soemtimes no response should be sent.
	// Base value = 0;
	// if value < 1, do not send a response
	// -1 = once set, value cannot be changed and sending any response to this client wont ever be possible. 
	int _responseToBeSent; // Set once the _responseBuff has been filled with a HttpResponse _finalResponse.
	bool _responseSent; // The response has been sent to this client, if _keepAlive, should be reset (i guess)
	bool _keepAlive;
	bool _readyToReceive; // Client is waiting for response
	bool _toBeClosed; // CLient should be closed, can be set by EPOLLHUP

	std::time_t _timestamp; // Client last action, used for TIMEOUT

	Client &operator=(const Client &obj);
	
public:
	Client(int fd);
	Client(int fd, sockaddr_in& clientAddr);
	Client(const Client &obj);
	~Client();

	int getFD() const;
	int &getRefFD();

	std::string &getBuffer();
	void setBuffer(const std::string &input);
	
	char *getTmpBufferPtr();
	
	bool isKeepAlive() const;
	void setKeepAlive(bool status);
	
	void setDoneReceiving(bool status);
	bool doneReceiving() const;

	bool responseToBeSent() const;
	void setResponseToBeSent(int status);	

	bool readyToReceive() const;
	void setReadyToReceive(bool status);	

	bool responseSent() const;
	void setResponseSent(bool status);	

	void setToBeClosed(bool status);
	bool toBeClosed() const;

	void updateTimestamp();
	const std::time_t &getTimestamp() const;
	const std::time_t &getTimestampInSeconds() const;

	void setResponseBuff(const std::string &response);
	const std::string &getResponseBuff() const;

	std::size_t getBytesSent() const;
	void setBytesSent(std::size_t bytes);
	void addBytesSent(std::size_t bytes);

	void refreshClient();

	std::string bufferManager();

	void printStatus() const;
};

#endif
