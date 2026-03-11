#include "Client.hpp"
#include <iostream>
#include "terminalColors.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Client::Client(int fd) : _clientFD(fd), _doneReceiving(false) {
	std::cout << ORANGE << "NEW CLIENT FD = " << fd << RESET << std::endl;
}

Client::~Client() {
	std::cout << RED << "Client destructor" << RESET << std::endl;
//	close(_clientFD); 
}

Client::Client(const Client &obj) : _clientFD(obj._clientFD) { 
	std::cout << PINK << "Client copy constructor" << RESET << std::endl;
	std::memcpy(_tmpBuff, obj._tmpBuff, BUFFERSIZE);
	_buffer = obj._buffer;
};

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

// Undefined behavior / deprecated
Client &Client::operator=(const Client &obj)
{
	std::cout << PINK << "Client = operator" << RESET << std::endl;
	(void)obj;
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

bool Client::doneReceiving() const { 
	std::cout << "Done receiving :)" << std::endl;
	return _doneReceiving; }

void Client::setReceivingStatus(bool status) {
	_doneReceiving = status;
	std::cout << "Done receiving status is: " << status << std::endl; } 

void Client::setKeepAlive(bool status) { _keepAlive = status; }
bool Client::isKeepAlive() const { return _keepAlive; }

void Client::setResponseSent(bool status) { _responseSent = status; }
bool Client::responseSent() const { return _responseSent; }

/**
 * @brief If not keepAlive and is doneReceiving, the connexion should be closed.
 */
bool Client::toBeClosed() const
{
	if (!isKeepAlive() && responseSent())
		return true;
	return false;
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
