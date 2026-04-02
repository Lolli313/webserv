#pragma once

#include "TerminalColors.hpp"
#include "Tools.hpp"
#include "HttpTools.hpp"
#include "Cookie.hpp"

#include <set>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <fcntl.h>
#include <sstream>
#include <cstdlib>
#include <unistd.h>


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

public:
  // CONSTRUCTORS

  HttpRequest();
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

  // FUNCTIONS

  void cleanPath();
  void parseQueryParams();
  void parse(const std::string &request);
  void cookie(Cookie &cookie);
  void executeScript();
  // void executeResponse();
  void print() const;
};
