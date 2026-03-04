#pragma once

#include <set>
#include <map>
#include <string>
#include <vector>

#include "Tools.hpp"
#include "HttpTools.hpp"

#define URI_MAX_LENGTH // To be defined if any.

class HttpRequest {

  private:

    std::string _methodStr;
    std::string _path;
    std::string _purePath;
    std::map<std::string, std::string> _queryParams;
    std::string _httpVersion;
    std::map<std::string, std::string> _header;
    std::string _body;

    bool isValidHttpVersion(const std::string &version) const;
    bool isPathSafe(const std::string &path) const;
    void parseQueryParams();

  public:

    // CONSTRUCTORS

    HttpRequest() {}
    ~HttpRequest() {}

    // GETTERS

    const std::string &getMethodStr() const { return _methodStr;}
    const std::string &getPath() const {return _path;}
    const std::string &getPurePath() const {return _purePath;}
    const std::map<std::string, std::string> &getQueryParams() const {return _queryParams;}
    const std::string &getHttpVersion() const {return _httpVersion;}
    const std::map<std::string, std::string> &getHeader() const {return _header;}
    const std::string &getBody() const {return _body;}

    // FUNCTIONS

    void print() const;
    void parse(const std::string &rawRequest);
};
