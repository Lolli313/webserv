#pragma once

#include "terminalColors.hpp"
#include "Tools.hpp"
#include "HttpTools.hpp"

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
#include <sstream>
#include <cstdlib>

class HttpRequest {

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

    const std::string &getMethodStr() const { return _methodStr;}
    const std::string &getPath() const {return _path;}
    const std::string &getPurePath() const {return _purePath;}
    const std::map<std::string, std::string> &getQueryParams() const {return _queryParams;}
    const std::string &getHttpVersion() const {return _httpVersion;}
    const std::map<std::string, std::string> &getHeader() const {return _header;}
    const std::string &getBoundary() const {return _boundary;}
    const std::string &getBody() const {return _body;}

    // FUNCTIONS

    void parseQueryParams();
    void parse(const std::string &request);
    void print() const;
};
