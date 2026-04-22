#pragma once

#include "TerminalColors.hpp"
#include "HttpTools.hpp"
#include "Cookie.hpp"
#include "Tools.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cerrno>
#include <cctype>
#include <set>
#include <map>


class Post;

class HttpRequest
{

private:
	std::string _methodStr;
	std::string _path;
	std::string _purePath;
	std::map<std::string, std::string> _queryParams;
	std::string _httpVersion;
	std::map<std::string, std::string> _header;
	std::string _boundary;
	std::string _body;

	sockaddr_in _clientAddr;
	std::string _clientIP;
	std::string _port;
	std::string _serverName;

public:
	// CONSTRUCTORS

	HttpRequest();
	HttpRequest(const sockaddr_in& clientAddr);
	HttpRequest(const HttpRequest &other);
	HttpRequest &operator=(const HttpRequest &other);
	~HttpRequest();

	// GETTERS

	const std::string &getMethod() const { return _methodStr; }
	const std::string &getPath() const { return _path; }
	const std::string &getPurePath() const { return _purePath; }
	const std::map<std::string, std::string> &getQueryParams() const { return _queryParams; }
	const std::string &getHttpVersion() const { return _httpVersion; }
	const std::map<std::string, std::string> &getHeader() const { return _header; }
	const std::string &getBoundary() const { return _boundary; }
	const std::string &getBody() const { return _body; }
	const sockaddr_in &getClientAddr() const { return _clientAddr; }
	const std::string& getClientIP();
	const std::string& getPort() const { return _port; }
	const std::string& getServerName() const { return _serverName; }

	// SETTERS

	void setPort(const std::string& port) { _port = port; }
	void setServerName(const std::string& serverName) { _serverName = serverName; }
	
	// FUNCTIONS
	
	const std::string findHeader(const std::string& key) const;
	void cleanPath();
	void parseQueryParams();
	void parse(const std::string &request);
	void cookie(Cookie &cookie);
	void executeScript();
	void print() const;
};
