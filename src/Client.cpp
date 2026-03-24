#include "Client.hpp"
#include <iostream>
#include "terminalColors.hpp"
#include "Tools.hpp"
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <string>

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

int Client::getFD() const { return _clientFD; }

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
	_buffer.clear();
	_responseBuff.clear();
}

std::string Client::bufferManager() {
	
	// Check la position dela request dans le buffer pour pouvoir isoler la request
	size_t posGet = _buffer.find("GET ");
	size_t posPost = _buffer.find("POST ");
	size_t posDelete = _buffer.find("DELETE ");
	
	// Isole le debut de la request pour analyser la suite
	size_t pos = std::min(posGet, posPost);
	size_t minPos = std::min(posDelete, pos);
	if (minPos == std::string::npos) {
		_buffer.erase();
		return "";
	}
	_buffer.erase(0, minPos);

	// maintenant on verifie si la partie des headers est finit et note le debut du body
	size_t posHeaderEnd = _buffer.find("\r\n\r\n");
	if (posHeaderEnd == std::string::npos) {
        return "";
    }
	std::string headers = _buffer.substr(0, posHeaderEnd);
	size_t posBodyStart = posHeaderEnd + 4;

	// la il faut trouver Content-Length pour savoir si le body est finit si il y en a un
	size_t posContentLengthStart = headers.find("Content-Length: ");
	if (posContentLengthStart == std::string::npos) {
		std::string request = _buffer.substr(0, posBodyStart);
        _buffer.erase(0, posBodyStart);
		setDoneReceiving(true);
		return request;
	}

	// si il y a un content length on verifie qu'il soit remplit
	size_t posContentLengthStop = headers.find("\r\n", posContentLengthStart);
	if (posContentLengthStop == std::string::npos) {
		throw Tools::Exception(400, "HttpRequest: Malformed body");
	}
	std::string contentLengthStr = headers.substr(posContentLengthStart + 16, posContentLengthStop - (posContentLengthStart + 16));
	char* endPtr;
	unsigned long contentLength = strtoul(contentLengthStr.c_str(), &endPtr, 10);
	if (*endPtr != '\0' && !isspace(*endPtr)) {
		throw Tools::Exception(400, "HttpRequest: Malformed body");
	}
    if (_buffer.length() >= posBodyStart + contentLength - 2) {
        std::string request = _buffer.substr(0, posBodyStart + contentLength);
        _buffer.erase(0, posBodyStart + contentLength);
		setDoneReceiving(true);
        return request;
    } else {
		// std::clog << _buffer.length() << " " << posBodyStart << " " << contentLength << std::endl;
        throw Tools::Exception(413, "HttpRequest: Malformed body");
    }
}
