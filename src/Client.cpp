#include "Client.hpp"
#include <iostream>
#include "terminalColors.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Client::Client(int fd) : _clientFD(fd), 
						_bytesSent(0),
						_doneReceiving(false), 
						_responseToBeSent(0), 
						_responseSent(false), 
						_keepAlive(true), 
						_readyToReceive(false), 
						_toBeClosed(false)
						{
	// std::clog << ORANGE << "NEW CLIENT FD = " << fd << RESET << std::endl;
}

Client::~Client() {
	// std::clog << RED << "Client destructor" << RESET << std::endl;
	// close(_clientFD); 
}

Client::Client(const Client &obj) : _clientFD(obj._clientFD),
								_bytesSent(obj._bytesSent),
								_doneReceiving(obj._doneReceiving),  
								_responseToBeSent(obj._responseToBeSent), 
								_responseSent(obj._responseSent), 
								_keepAlive(obj._keepAlive), 
								_readyToReceive(obj._readyToReceive), 
								_toBeClosed(obj._toBeClosed)
								{ 
	// std::clog << PINK << "Client copy constructor" << RESET << std::endl;
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
	// std::clog << PINK << "Client = operator" << RESET << std::endl;
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
void Client::setBuffer(const std::string &input) { _buffer = input; }

char *Client::getTmpBufferPtr() { return _tmpBuff; }
// chat *Client::getTmpBuffer() { return _tmpBuff; }

bool Client::doneReceiving() const { 
	// std::clog << "Done receiving :)" << std::endl;
	return _doneReceiving;
}

void Client::setDoneReceiving(bool status) {
	_doneReceiving = status;
	// std::clog << "Done receiving status is: " << status << std::endl;
}

void Client::setKeepAlive(bool status) { _keepAlive = status; }
bool Client::isKeepAlive() const { return _keepAlive; }

void Client::setResponseSent(bool status) { _responseSent = status; }
bool Client::responseSent() const { return _responseSent; }

/**
 * @brief 
 * @param status 
 * { 1 = toBeSent, 
 *   0 = not toBeSent, 
 *   -1 = no respopnse should be sent, and the value cannot be changed. It means the end of the connection, for EPOLLHUP}
 */
void Client::setResponseToBeSent(int status) { 
	if (_responseToBeSent != -1)
		_responseToBeSent = status; 
	}
bool Client::responseToBeSent() const { 
	if (_responseToBeSent > 0)
		return true;
	return false;
}

void Client::setReadyToReceive(bool status) { _readyToReceive = status; }
bool Client::readyToReceive() const { return _readyToReceive; }

void Client::setToBeClosed(bool status) { _toBeClosed = status; }

/**
 * @brief Check the conditions to determine if the client connection should be closed. 
 */
bool Client::toBeClosed() const
{
	if (_toBeClosed)
		return true;
	else if (!isKeepAlive() && responseSent())
		return true;
	return false;
}

const std::string &Client::getResponseBuff() const { return _responseBuff; }
void Client::setResponseBuff(const std::string &response) { _responseBuff = response; }

std::size_t Client::getBytesSent() const { return _bytesSent; }
// Overwrite the _bytesSent by the input
void Client::setBytesSent(std::size_t bytes) { _bytesSent = bytes; }
// Add the bytes to the total bytesSent
void Client::addBytesSent(std::size_t bytes) { _bytesSent += bytes; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

/**
 * @brief Reset the flags for the next request.
 * @note Do not reset _toBeClosed nor _keepAlive.
 */
void Client::refreshClient()
{
	_bytesSent = 0;
	_doneReceiving = false;
	_responseToBeSent = 0;
	_responseSent = false;
	_readyToReceive = false;
}

void Client::bufferManager() {

	// std::clog << "BEFORE" << std::endl;
	// std::clog << BLUE << _buffer << RESET << std::endl;
	// std::clog << GREEN << _tmpBuff << RESET << std::endl;

	const char* methods[] = {"GET ", "POST ", "DELETE "};
	const size_t num_methods = sizeof(methods) / sizeof(methods[0]);
	size_t earliest_pos = std::string::npos;

	for (size_t i = 0; i < num_methods; ++i) {
		size_t pos = _buffer.find(methods[i]);
		if (pos != std::string::npos && (earliest_pos == std::string::npos || pos < earliest_pos)) {
			earliest_pos = pos;
		}
	}

	if (earliest_pos != std::string::npos) {
		_buffer.erase(0, earliest_pos);
	} else {
		_buffer.erase();
	}

	// std::clog << std::endl;
	// std::clog << "AFTER" << std::endl;
	// std::clog << BLUE << _buffer << RESET << std::endl;
	// std::clog << GREEN << _tmpBuff << RESET << std::endl;
}
