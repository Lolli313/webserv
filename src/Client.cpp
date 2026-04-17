#include "Client.hpp"
#include <iostream>
#include "TerminalColors.hpp"
#include "Tools.hpp"
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

Client::Client(int fd) :
	_clientFD(fd), 
	_bytesSent(0),
	_doneReceiving(false), 
	_responseToBeSent(0), 
	_responseSent(false), 
	_keepAlive(true), 
	_readyToReceive(false), 
	_toBeClosed(false),
	_timestamp(std::time(0)),
	_connectedPort(-1)
{
	LOG(INFO, CYAN_BRIGHT, "NEW CLIENT FD", Tools::intToString(fd));
}

Client::Client(int fd, sockaddr_in& clientAddr) :
	_clientFD(fd),
	_clientAddr(clientAddr),
	_bytesSent(0),
	_doneReceiving(false), 
	_responseToBeSent(0), 
	_responseSent(false), 
	_keepAlive(true), 
	_readyToReceive(false), 
	_toBeClosed(false),
	_timestamp(std::time(0)),
	_connectedPort(-1)
{
	LOG(INFO, CYAN_BRIGHT, "NEW CLIENT FD", Tools::intToString(fd));
}

Client::~Client() {
	LOG(INFO, RED_BRIGHT, "Client destructor");
	// close(_clientFD); 
}

Client::Client(const Client &obj) :
	_clientFD(obj._clientFD),
	_bytesSent(obj._bytesSent),
	_doneReceiving(obj._doneReceiving),  
	_responseToBeSent(obj._responseToBeSent), 
	_responseSent(obj._responseSent), 
	_keepAlive(obj._keepAlive), 
	_readyToReceive(obj._readyToReceive), 
	_toBeClosed(obj._toBeClosed),
	_timestamp(obj._timestamp)
{ 
	LOG(INFO, PINK, "Client copy constructor");
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
	LOG(INFO, PINK, "Client = operator");
	(void)obj;
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Client::getFD() const { return _clientFD; }
int &Client::getRefFD() { return _clientFD; }

std::string &Client::getBuffer() { return _buffer; }
void Client::setBuffer(const std::string &input) { _buffer = input; }

sockaddr_in Client::getClientAddr() const { return _clientAddr; }

const std::string& Client::getPath() const { return _path; }
const std::string& Client::getHost() const { return _host; }

long Client::getMaxBodySize() const { return _maxBodySize; }
void Client::setMaxBodySize(long src) { _maxBodySize = src; }

char *Client::getTmpBufferPtr() { return _tmpBuff; }
// chat *Client::getTmpBuffer() { return _tmpBuff; }

bool Client::doneReceiving() const { 
	LOG(INFO, LIGHT_GRAY, "Done receiving", Tools::boolToString(_doneReceiving));
	return _doneReceiving;
}

void Client::setDoneReceiving(bool status) {
	_doneReceiving = status;
	LOG(INFO, LIGHT_GRAY, "Done receiving status is", Tools::boolToString(status));
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
 *   -1 = no response should be sent, and the value cannot be changed. It means the end of the connection, for EPOLLHUP}
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

// Set _timestamp to current time.
void Client::updateTimestamp() { _timestamp = std::time(0); }
const std::time_t &Client::getTimestamp() const { return _timestamp; }
const std::time_t &Client::getTimestampInSeconds() const { return _timestamp; }
int Client::getConnectedPort() { 
	if (_connectedPort >= 0)
		return _connectedPort;
	sockaddr_in serverAddr;
	socklen_t len = sizeof(serverAddr);
	
	getsockname(_clientFD, (sockaddr*)&serverAddr, &len);
	_connectedPort = ntohs(serverAddr.sin_port);
	return _connectedPort;
}

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

void Client::findPath(std::string firstLine) {
	std::istringstream iss(firstLine);
    std::string method;
    std::string protocol;

    iss >> method >> _path >> protocol;
}

void Client::findHost(std::string headers) {
	size_t hostPos = headers.find("host: ");
	if (hostPos != std::string::npos) {
        hostPos += 6;
        size_t endPos = headers.find_first_of(" \r\n", hostPos);
        if (endPos != std::string::npos) {
            _host = headers.substr(hostPos, endPos - hostPos);
        } else {
            _host = headers.substr(hostPos);
        }
    }
}

void Client::bufferManager() {
	// Check la position dela request dans le buffer pour pouvoir isoler la request
	LOG(DEBUG, YELLOW_BRIGHT, "_buffer is " + _buffer);
	const char* methods[] = {"GET ", "HEAD ", "POST ", "PUT ", "DELETE ", "OPTIONS ", "TRACE ", "CONNECT "};
	std::vector<std::string> request(methods, methods + sizeof(methods)/sizeof(methods[0]));
	std::size_t minPos = std::string::npos;
	for (std::vector<std::string>::const_iterator it = request.begin(); it != request.end(); ++it) {
		std::size_t pos = _buffer.find(*it);
		if (pos != std::string::npos && minPos > pos) {
			minPos = pos;
		}
	}
	if (minPos == std::string::npos) {
		throw Tools::Exception(400, "Inexisting method");
	}
	_buffer.erase(0, minPos);

	// maintenant on verifie si la partie des headers est finit et note le debut du body
	std::size_t posHeaderStart = _buffer.find_first_of("\r\n");

	// on trouve le path
	findPath(_buffer.substr(0, posHeaderStart));

	_posHeaderEnd = _buffer.find("\r\n\r\n");
	if (posHeaderStart != std::string::npos && _posHeaderEnd != std::string::npos) {
		bool flag = false;
		for (std::size_t i = posHeaderStart; i < _posHeaderEnd; ++i) {
			if (_buffer[i] == ':') {
				flag = true;
			} else if (_buffer[i] == '\r') {
				flag = false;
			}
			if (flag == false) {
				_buffer[i] = std::tolower(_buffer[i]);
			}
		}
	}
	// transform en minuscule
	if (posHeaderStart == std::string::npos || _posHeaderEnd == std::string::npos) {
        return ;
    }
	_headers = _buffer.substr(0, _posHeaderEnd);

	// on trouve le host
	findHost(_headers);
}

std::string Client::bodyVerification() {
	std::size_t posBodyStart = _posHeaderEnd + 4;
	// la il faut trouver Content-Length pour savoir si le body est finit si il y en a un
	std::size_t posContentLengthStart = _headers.find("content-length: ");
	if (posContentLengthStart == std::string::npos) {
		// std::string request = _buffer.substr(0, posBodyStart);
        // _buffer.erase(0, posBodyStart);
		setDoneReceiving(true);
		return _buffer;
	}
	// si il y a un content length on verifie qu'il soit remplit
	std::size_t posContentLengthStop = _headers.find("\r\n", posContentLengthStart);
	if (posContentLengthStop == std::string::npos) {
		posContentLengthStop = _posHeaderEnd;
		// throw Tools::Exception(400, "HttpRequest: Malformed body");
	}
	std::string contentLengthStr = _headers.substr(posContentLengthStart + 16, posContentLengthStop - (posContentLengthStart + 16));
	char* endPtr;
	unsigned long contentLength = strtoul(contentLengthStr.c_str(), &endPtr, 10);
	if (*endPtr != '\0' && !isspace(*endPtr)) {
		throw Tools::Exception(400, "HttpRequest: Malformed body");
	}
	// LOG(DEBUG, PINK, "contentLength is " + Tools::intToString(contentLength));
	// LOG(DEBUG, PINK, "_maxBodySize is " + Tools::intToString(_maxBodySize));
	std::string body = _buffer.substr(posBodyStart);
	if (contentLength == body.size()) {
		LOG(DEBUG, PINK, Tools::intToString(body.size()));
		LOG(DEBUG, PINK, Tools::intToString(_buffer.size()));
		LOG(DEBUG, PINK, Tools::intToString(posBodyStart));
		LOG(DEBUG, PINK, Tools::intToString(contentLength));
		LOG(DEBUG, PINK, Tools::intToString(_maxBodySize));
		setDoneReceiving(true);
		return _buffer;
	}
    else if (_buffer.size() - posBodyStart - 2 == contentLength && contentLength < _maxBodySize) {
		LOG(DEBUG, PINK, Tools::intToString(body.size()));
		LOG(DEBUG, PINK, Tools::intToString(_buffer.size()));
		LOG(DEBUG, PINK, Tools::intToString(posBodyStart));
		LOG(DEBUG, PINK, Tools::intToString(contentLength));
		LOG(DEBUG, PINK, Tools::intToString(_maxBodySize));
        return _buffer;
    } else {
		LOG(DEBUG, PINK, Tools::intToString(body.size()));
		LOG(DEBUG, PINK, Tools::intToString(_buffer.size()));
		LOG(DEBUG, PINK, Tools::intToString(posBodyStart));
		LOG(DEBUG, PINK, Tools::intToString(contentLength));
		LOG(DEBUG, PINK, Tools::intToString(_maxBodySize));
        throw Tools::Exception(413, "Wrong content size");
    }
}
