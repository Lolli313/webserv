#include "HttpRequest.hpp"
#include "Post.hpp"
#include "Cookie.hpp"
#include "HttpResponse.hpp"
#include <sys/wait.h>
#include <ctime>
#include <iomanip>
#include <sstream>

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

	// parse la methode, le path et la version du http
	std::istringstream iss(request);
	// std::clog << request << std::endl;
	if (!(iss >> _methodStr >> _path >> _httpVersion)) {
    	throw Tools::Exception(400, "HttpRequest: Malformed request");
	}
	if (_methodStr != "GET" && _methodStr != "POST" && _methodStr != "DELETE") {
		// std::clog << LIGHT_BLUE << "HttpRequest: Unknown method" << RESET << std::endl;
    	throw Tools::Exception(405, "HttpRequest: Unknown method");
	}
	if (_path.find("/../") != std::string::npos || _path.find("//") != std::string::npos || _path.empty()) {
		throw Tools::Exception(403, "HttpRequest: Wrong path request");
	}
	if (_httpVersion != "HTTP/1.0" && _httpVersion != "HTTP/1.1") {
    	throw Tools::Exception(505, "HttpRequest: Neither http1.0 nor http1.1");
	}

	// parse les query params grace a la fonction ET RENVOIE LE PURE-PATH DONC LE PATH SANS LES QUERY PARAMS
	parseQueryParams();
	if (_purePath.find("..") != std::string::npos || _purePath.find("//") != std::string::npos || _purePath.empty()) {
    	throw Tools::Exception(400, "HttpRequest: Wrong query params");
	}

	// parse les headers
	std::string line;
  	HttpTools tools;
	if (std::getline(iss, line) && line != "\r") {
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
	std::map<std::string, std::string>::const_iterator itContentType = _header.find("content-type");
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

	// stock le body pour post
	std::stringstream bodyStream;
	while (std::getline(iss, line)) {
    	bodyStream << line << "\n";
	}
	_body = bodyStream.str();
}

void HttpRequest::cookie(Cookie &cookie) {
	std::map<std::string, std::string>::const_iterator itCookie = _header.find("cookie");
	if (itCookie != _header.end()) {
		cookie.setCookie(itCookie->second);
	}
}

// void HttpRequest::executeMethod() {
// 	if (_methodStr == "GET") {
// 		// std::clog << "code pour get" << std::endl;
// 	} else if (_methodStr == "POST") {
// 		Post post(*this);
// 		post.parseBody();
// 		post.saveInFile();
// 	} else if (_methodStr == "DELETE") {
// 		int fd = open(_path.c_str(), O_RDONLY);
// 		if (fd == -1) {
// 			throw Tools::Exception(500, "existe pas ou pas accessible");
// 		} else {
// 			std::remove(_path.c_str());
// 		}
// 		close(fd);
// 	}
// }

void HttpRequest::executeScript() {
	if (_purePath != "cgi-bin/hello.py" && _purePath != "cgi-bin/info.php") {
		// std::clog << "marche pas" << std::endl;
		return;
	}

    int pipefd[2];
    if (pipe(pipefd) == -1) {
		// std::clog << "NULL1" << std::endl; 
        // throw std::runtime_error("Failed to create pipe");
    }
    pid_t pid = fork();
    if (pid == -1) {
		// std::clog << "NULL2" << std::endl; 
        // throw std::runtime_error("Failed to fork");
    } else if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // for (const auto &[key, value] : env) {
        //     setenv(key.c_str(), value.c_str(), 1);
        // }

        execl(_purePath.c_str(), _purePath.c_str(), NULL);
		// std::clog << "NULL3" << std::endl; 
        exit(1);
    } else { 
        close(pipefd[1]);

        char buffer[4096];
        std::string output;
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytesRead);
        }
        close(pipefd[0]);

        // int status;
        // waitpid(pid, &status, 0);
        // if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
		// 	// std::clog << "NULL4" << std::endl; 
        //     // throw std::runtime_error("CGI script execution failed");
        // }
		// std::clog << YELLOW << "CGI Output: " << output << RESET << std::endl;
    }
}

// void HttpRequest::executeResponse() {
// 	HttpResponse response;
// 	if (_methodStr == "GET") {
// 		response.setReturnCode(200);
// 		response.setReturnMessage("Ok");
// 	} else if (_methodStr == "POST") {
// 		response.setReturnCode(201);
// 		response.setReturnMessage("Created");
// 	} else if (_methodStr == "DELETE") {
// 		response.setReturnCode(204);
// 		response.setReturnMessage("No Content");
// 	}
// 	response.setHttpVersion(_httpVersion);
// 	for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it) {
// 		if (it->first == "Content-Length" || it->first == "Content-Type" || it->first == "Connection"
// 			|| it->first == "Server" || it->first == "Cache-Control" || it->first == "Cookie") {
// 				if (it->first == "Cookie") {
// 					response.addHeader("Set-Cookie", it->second);
// 				} else {
// 					response.addHeader(it->first, it->second);
// 				}
// 			}
// 	}
// 	response.addDateHeader();
// 	response.setBody(_body);
// 	// std::clog << RED << response.getFinalResponse() << RESET << std::endl;
// }

void HttpRequest::print() const {
	// std::clog << YELLOW << "Method : " << RESET << _methodStr << std::endl;
	// std::clog << YELLOW << "Path : " << RESET << _path << std::endl;
	// std::clog << YELLOW << "Pure Path : " << RESET << _purePath << std::endl;
	// std::clog << YELLOW << "Query Params : " << RESET << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _queryParams.begin();
		it != _queryParams.end(); ++it) {
		// std::clog << " " << it->first << " : " << it->second << std::endl;
	}
	// std::clog << YELLOW << "HTTP Version : " << RESET << _httpVersion << std::endl;
	// std::clog << YELLOW << "Headers : " << RESET << std::endl; 
	for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it) {
		// std::clog << " " << it->first << " : " << it->second << std::endl;
	}
	// std::clog << YELLOW << "Boundary : " << RESET << _boundary << std::endl;
	// std::clog << YELLOW << "Body : " << std::endl << RESET << _body;
}
