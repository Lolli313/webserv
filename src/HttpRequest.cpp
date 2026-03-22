#include "../include/HttpRequest.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

HttpRequest::HttpRequest() : _headersParsed(false) {}

// 1. Parse the request.
// 2. Analyse and throw errors accordingly if found.
HttpRequest::HttpRequest(const std::string &request)
{
	(void)request;
};

HttpRequest::~HttpRequest() {};

HttpRequest::HttpRequest(const HttpRequest &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
HttpRequest &HttpRequest::operator=(const HttpRequest &obj)
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

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool HttpRequest::hasBody() { return !_body.empty(); }

bool HttpRequest::parse(const std::string &request) {
	(void)request;
	std::vector<std::string> split = Tools::splitString(request);
	split[0].resize(split[0].size() - 1);
	_header[split[0]] = split[1];
	_headersParsed = true;
	return true;
}