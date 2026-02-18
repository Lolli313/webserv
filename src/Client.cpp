#include "Client.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Client::Client(int fd) : _clientFD(fd), _doneReceiving(false) {}

Client::~Client() { close(_clientFD); }

Client::Client(const Client &obj) : _clientFD(obj._clientFD) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

// Undefined behavior / deprecated
Client &Client::operator=(const Client &obj)
{
	if (this != &obj)
	{
		// _buffer = obj._buffer;
		// _doneReceiving = obj._doneReceiving;
	}
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Client::getFD() { return _clientFD; }

std::string &Client::getBuffer() { return _buffer; }

char *Client::getTmpBufferPtr() { return _tmpBuff; }
// chat *Client::getTmpBuffer() { return _tmpBuff; }

bool Client::doneReceiving() const { return _doneReceiving; }

void Client::setReceivingStatus(bool status) { _doneReceiving = status; } 

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
