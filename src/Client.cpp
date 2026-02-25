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
	std::cout << RED << "Closing client fd = " << _clientFD << RESET << std::endl;
//	close(_clientFD); 
}

Client::Client(const Client &obj) : _clientFD(obj._clientFD) { 
	std::cout << PINK << "Client copy constructor" << RESET << std::endl;
	// *this = obj; 
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

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
