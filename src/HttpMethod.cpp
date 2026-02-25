#include "HttpMethod.hpp"

HttpMethod::HttpMethod() {}

HttpMethod::HttpMethod(const std::string &request)  {
    parse(request);
}

HttpMethod::~HttpMethod() {}

void HttpMethod::parse(const std::string &request) {

    parseRequest(request);
}

void HttpMethod::parseRequest(const std::string &request) {

    std::istringstream iss(request);
    std::string line;

    if (!std::getline(iss, line)) {
        throw std::runtime_error("ERROR : INVALID REQUEST");
    }

    std::istringstream first(line);
    if (!(first >> _method >> _path)) {
        throw std::runtime_error("ERROR : INVALID REQUEST");
    }

    while (std::getline(iss, line) && line != "\r") {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            _header[key] = value;
        }
    }

    std::ostringstream body;
    while (std::getline(iss, line)) {
        body << line << "\n";
    }
    _body = body.str();
    if (!_body.empty()) {
        _body.erase(_body.size() - 1);
    }
}

const std::string &HttpMethod::getMethod() const {
    return _method;
}

const std::string &HttpMethod::getPath() const {
    return _path;
}

const std::map<std::string, std::string> &HttpMethod::getHeader() const {
    return _header;
}

const std::string &HttpMethod::getBody() const {
    return _body;
}
