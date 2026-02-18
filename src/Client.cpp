#include "Client.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Client::Client(int fd) : _clientFD(fd), _doneReceiving(false) {}

Client::~Client() { close(_clientFD); }

Client::Client(const Client &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
Client &Client::operator=(const Client &obj)
{
	if (this != &obj)
	{
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

char *Client::getBufferEnd() { return &_buffer.back(); }

bool Client::getReceivingStatus() const { return _doneReceiving; } 
void Client::setReceivingStatus(bool status) { _doneReceiving = status; } 

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
