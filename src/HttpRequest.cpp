#include <algorithm>
#include <iostream>
#include <sstream>
#include <cctype>

#include "HttpRequest.hpp"

bool HttpRequest::isValidHttpVersion(const std::string &version) const {
  return version == "HTTP/1.0" || version == "HTTP/1.1";
}

bool HttpRequest::isPathSafe(const std::string &path) const {
  if (path.find("/../") != std::string::npos) {
    return false;
  }
  if (path.find("//") != std::string::npos) {
    return false;
  }
  if (path.empty() || path[0] != '/') {
    return false;
  }
  return true;
}

void HttpRequest::parseQueryParams() {
  size_t queryPos = _path.find('?');
  if (queryPos != std::string::npos) {
    _purePath = _path.substr(0, queryPos);
    std::string queryStr = _path.substr(queryPos + 1);
    std::stringstream ss(queryStr);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
      size_t equal_pos = pair.find('=');
      if (equal_pos != std::string::npos) {
          std::string key = pair.substr(0, equal_pos);
          std::string value = pair.substr(equal_pos + 1);
          _queryParams[key] = value;
      }
    }
  } else {
    _purePath = _path;
  }
}

void HttpRequest::parse(const std::string &rawRequest) {
  std::istringstream iss(rawRequest);
  std::string line;

  if (!std::getline(iss, line)) {
    throw Tools::Exception(404, "WRONG METHOD");
  }

  std::istringstream requestLine(line);
  if (!(requestLine >> _methodStr >> _path >> _httpVersion)) {
    throw Tools::Exception(404, "WRONG METHOD");
  }

  if (_methodStr == "GET" || _methodStr == "POST" || _methodStr == "DELETE") {
      throw Tools::Exception(501, "Invalid HTTP method");
  }
  if (!isValidHttpVersion(_httpVersion)) {
      throw Tools::Exception(505, "HTTP version not supported");
  }

  parseQueryParams();

  if (!isPathSafe(_purePath)) {
      throw Tools::Exception(403, "Forbidden: unsafe path");
  }

  while (std::getline(iss, line) && line != "\r") {
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
      std::string key = line.substr(0, colonPos);
      std::string value = line.substr(colonPos + 1);
      value.erase(0, value.find_first_not_of(" \t"));

      if (!HttpTools::isValidMethod(key)) {
        throw Tools::Exception(404, "WRONG METHOD");
      }

      _header[key] = value;
    }
  }

  std::map<std::string, std::string>::const_iterator it = _header.find("Content-Length");
  if (it != _header.end()) {
    int contentLength = atoi(it->second.c_str());
    if (contentLength > 0) {
      char *bodyBuffer = new char[contentLength + 1];
      iss.read(bodyBuffer, contentLength);
      bodyBuffer[contentLength] = '\0';
      _body.assign(bodyBuffer, contentLength);
      delete[] bodyBuffer;
    }
  }
}

void HttpRequest::print() const {
  std::cout << "Method: " << _methodStr << "\n";
  std::cout << "Path: " << _path << "\n";
  std::cout << "Pure Path: " << _purePath << "\n";
  std::cout << "Query Params:\n";
  for (std::map<std::string, std::string>::const_iterator it = _queryParams.begin();
        it != _queryParams.end(); ++it) {
      std::cout << " " << it->first << ": " << it->second << "\n";
  }
  std::cout << "HTTP Version: " << _httpVersion << "\n";
  std::cout << "Headers:\n";
  for (std::map<std::string, std::string>::const_iterator it = _header.begin();
        it != _header.end(); ++it) {
      std::cout << "  " << it->first << ": " << it->second << "\n";
  }
  if (!_body.empty()) {
      std::cout << "Body:\n" << _body << "\n";
  }
}
