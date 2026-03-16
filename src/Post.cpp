#include "Post.hpp"

/*
================================================================================
======= CONSTRUCTORS // DESTRUCTOR =============================================
================================================================================
*/

Post::Post() : _request() {}

Post::Post(HttpRequest &request) : _request(request) {}

Post::Post(const Post &other) : _request(other._request) {}

Post &Post::operator=(const Post &other) {
    if (this != &other) {
        _request = other._request;
    }
    return *this;
}

Post::~Post() {}

/*
================================================================================
======= FUNCTIONS ==============================================================
================================================================================
*/

void Post::parseBody() {
    std::string line;
    std::string part;
    std::string boundary = _request.getBoundary();
    std::istringstream iss(_request.getBody());
    bool inPart = false;

    if (boundary.empty()) {
        std::map<std::string, std::string> head;
        std::map<std::string, std::string>::const_iterator it = _request.getHeader().find("Content-Type");
        if (it != _request.getHeader().end()) {
            head["Content-Type"] = it->second;
        }
        it = _request.getHeader().find("Content-Disposition");
        if (it != _request.getHeader().end()) {
            head["Content-Disposition"] = it->second;
        } else {
            head["Content-Disposition"] = "name=\"post\"";
        }
        head["Body"] = _request.getBody();
        _header.push_back(head);
        return;
    }

    while (std::getline(iss, line)) {

        if (line == boundary + "\r" || line == boundary + "--\r") {
            if (inPart && !part.empty()) {

                std::string currLine;
                std::istringstream currStream(part);
                std::map<std::string, std::string> head;

                while (std::getline(currStream, currLine) && !line.empty() && currLine != "\r") {
                    size_t pos = currLine.find(':');
                    if (pos != std::string::npos) {
                        std::string key = currLine.substr(0, pos);
                        std::string value = currLine.substr(pos + 1);
                        value.erase(0, value.find_first_not_of(" \t"));
                        head[key] = value;
                    }
                }

                std::stringstream bodyStream;
                while (std::getline(currStream, currLine)) {
                    bodyStream << currLine << "\n";
                }
                std::string body = bodyStream.str();
                if (!body.empty() && body[body.size() - 1] == '\n') {
                    body.erase(body.size() - 1);
                }
                head["Body"] = body;
                _header.push_back(head);
                part.erase();
            }

            if (line == boundary + "--\r") {
                break;
            }
            inPart = true;
            continue;
        }
        if (inPart) {
            part += line + "\n";
        }
    }
}

void Post::print() const {
    for (size_t i = 0; i < _header.size(); ++i) {
        std::cout << BLUE << "PART : " << i + 1 << RESET << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = _header[i].begin();
            it != _header[i].end(); ++it) {
            std::cout << YELLOW << it->first << " : " << RESET;
            if (it->first == "Body") {
                std::cout << std::endl;
            }
            std::cout << it->second << std::endl;
        }
    }
	
}

void Post::saveInFile() const {
    for (size_t i = 0; i < _header.size(); ++i) {

        std::map<std::string, std::string>::const_iterator it = _header[i].find("Content-Disposition");
        if (it == _header[i].end()) {
            throw Tools::Exception(400, "Post: No content disposition");
        }
        std::string filename;
        const std::string& name = it->second;
        size_t start = name.find("name=\"") + 6;
        size_t end = name.find("\"", start);
        if (start == std::string::npos || end == std::string::npos) {
            filename = "post";
        } else {
            filename = name.substr(start, end - start);
        }

        it = _header[i].find("Content-Type");
        if (it != _header[i].end()) {
            const std::string& format = it->second;
            size_t start = format.find("/");
            std::string formatType = format.substr(start + 1);
            size_t end = formatType.find_first_of(" \r\n\t;");
            if (start == std::string::npos || end == std::string::npos) {
                formatType = "txt";
            }
            formatType.erase(end);
            filename += '.' + formatType;
        }
        std::ofstream outFile(("files/" + filename).c_str(), std::ios::out | std::ios::binary);
        if (!outFile) {
            throw Tools::Exception(500, "Post: Can't create file");
        }
        it = _header[i].find("Body");
        outFile << it->second;
        outFile.close();
    }
}
