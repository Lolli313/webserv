#include "HttpRequest.hpp"

/*
================================================================================
======= CONSTRUCTORS // DESTRUCTOR =============================================
================================================================================
*/


HttpRequest::HttpRequest() :
	_methodStr(""),
	_path(""),
	_purePath(""),
	_queryParams(),
	_httpVersion(""),
	_header(),
	_boundary(""),
	_body("") {}

HttpRequest::HttpRequest(const HttpRequest &other) :
	_methodStr(other._methodStr),
	_path(other._path),
	_purePath(other._purePath),
	_queryParams(other._queryParams),
	_httpVersion(other._httpVersion),
	_header(other._header),
	_boundary(other._boundary),
	_body(other._body) {}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
	if (this != &other) {
		_methodStr = other._methodStr;
		_path = other._path;
		_purePath = other._purePath;
		_queryParams = other._queryParams;
		_httpVersion = other._httpVersion;
		_header = other._header;
		_boundary = other._boundary;
		_body = other._body;
	}
	return *this;
}

HttpRequest::~HttpRequest()	{}

/*
================================================================================
======= FUNCTIONS ==============================================================
================================================================================
*/

void HttpRequest::parseQueryParams() {
	size_t queryPos = _path.find('?');
	if (queryPos != std::string::npos) {
		_purePath = _path.substr(0, queryPos);
		std::string queryStr = _path.substr(queryPos + 1);
		std::stringstream ss(queryStr);
		std::string pair;
		while (std::getline(ss, pair, '&')) {
			size_t equalPos = pair.find('=');
			if (equalPos != std::string::npos) {
				std::string key = pair.substr(0, equalPos);
				std::string value = pair.substr(equalPos + 1);
				_queryParams[key] = value;
			}
		}
	} else {
		_purePath = _path;
	}
}

void HttpRequest::parse(const std::string &request) {

	// verifie que la requete n'est pas vide
	if (request.empty()) {
    throw Tools::Exception(400, "HttpRequest: Request is empty");
	}

	// parse la methode, le path et la version du http
	std::istringstream iss(request);
	if (!(iss >> _methodStr >> _path >> _httpVersion)) {
    throw Tools::Exception(400, "HttpRequest: Malformed request");
	}
	if (_methodStr != "GET" && _methodStr != "POST" && _methodStr != "DELETE") {
    throw Tools::Exception(501, "HttpRequest: Unknown method");
	}
	if (_path.find("/../") != std::string::npos || _path.find("//") != std::string::npos || _path.empty() || _path[0] != '/') {
    throw Tools::Exception(400, "HttpRequest: Wrong path request");
	}
	if (_httpVersion != "HTTP/1.0" && _httpVersion != "HTTP/1.1") {
    throw Tools::Exception(505, "HttpRequest: Neither http1.0 nor http1.1");
	}

	// parse les query params grace a la fonction ET RENVOIE LE PURE-PATH DONC LE PATH SANS LES QUERY PARAMS
	parseQueryParams();
	if (_purePath.find("/../") != std::string::npos || _purePath.find("//") != std::string::npos || _purePath.empty() || _purePath[0] != '/') {
    throw Tools::Exception(400, "HttpRequest: Wrong query params");
	}

	// parse les headers
	std::string line;
  HttpTools tools;
	if (!std::getline(iss, line)) {
    throw Tools::Exception(400, "HttpRequest: Malformed body");
	}
	while (std::getline(iss, line) && !line.empty() && line != "\r") {
		size_t pos = line.find(':');
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
      if (tools.isValidHttpRequestHeader(key)) {
        std::string value = line.substr(pos + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        _header[key] = value;
      }
		}
	}

	// trouve le boundary
	std::map<std::string, std::string>::const_iterator itContentType = _header.find("Content-Type");
	if (itContentType != _header.end()) {
		size_t boundaryPos = itContentType->second.find("boundary=");
		if (boundaryPos != std::string::npos) {
			_boundary = itContentType->second.substr(boundaryPos + 9);
			size_t endPos = _boundary.find_first_of(";\r\n ");
			if (endPos != std::string::npos) {
				_boundary = _boundary.substr(0, endPos);
			}
		}
	}

	// parse le body
	std::stringstream bodyStream;
	std::map<std::string, std::string>::const_iterator itContentLength = _header.find("Content-Length");
	while (std::getline(iss, line)) {
    	bodyStream << line << "\n";
	}
	_body = bodyStream.str();
	if (itContentLength != _header.end() && static_cast<long>(_body.size()) != std::atol(itContentLength->second.c_str())) {
    throw Tools::Exception(400, "HttpRequest: Wrong body size" + Tools::intToString(static_cast<int>(_body.size())));
	}
}

void HttpRequest::print() const {
	std::cout << YELLOW << "Method : " << RESET << _methodStr << std::endl;
	std::cout << YELLOW << "Path : " << RESET << _path << std::endl;
	std::cout << YELLOW << "Pure Path : " << RESET << _purePath << std::endl;
	std::cout << YELLOW << "Query Params : " << RESET << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _queryParams.begin();
		it != _queryParams.end(); ++it) {
		std::cout << " " << it->first << " : " << it->second << std::endl;
	}
	std::cout << YELLOW << "HTTP Version : " << RESET << _httpVersion << std::endl;
	std::cout << YELLOW << "Headers : " << RESET << std::endl; 
	for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it) {
		std::cout << " " << it->first << " : " << it->second << std::endl;
	}
	std::cout << YELLOW << "Boundary : " << RESET << _boundary << std::endl;
	std::cout << YELLOW << "Body : " << std::endl << RESET << _body;
}
