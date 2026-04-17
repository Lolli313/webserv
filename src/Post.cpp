#include "Post.hpp"

/*
================================================================================
======= CONSTRUCTORS // DESTRUCTOR =============================================
================================================================================
*/

Post::Post(const HttpRequest &request) : _request(request) {}

Post::Post(const Post &other) : _request(other._request), _header(other._header) {}

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
        std::map<std::string, std::string>::const_iterator it = _request.getHeader().find("content-type");
        if (it != _request.getHeader().end()) {
            head["content-type"] = it->second;
        }
        it = _request.getHeader().find("content-disposition");
        if (it != _request.getHeader().end()) {
            head["content-disposition"] = it->second;
        } else {
            throw Tools::Exception(400, "Post: No content-disposition");
            // head["Content-Disposition"] = "name=\"post\"";
        }
        head["body"] = _request.getBody();
        _header.push_back(head);
        return;
    }
    while (std::getline(iss, line)) {
        if (line == boundary + "\r" || line == boundary + "--\r" || line == "--" + boundary + "\r" || line == "--" + boundary + "--\r" || line == boundary + "--" || line == "--" + boundary + "--") {
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
                        Tools::transformStringToLowecase(key);
                        Tools::transformStringToLowecase(value);
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
                head["body"] = body;
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
        LOG(DEBUG, "PART : " + Tools::intToString(i + 1));
        for (std::map<std::string, std::string>::const_iterator it = _header[i].begin();
            it != _header[i].end(); ++it)
        {
            if (it->first == "body")
                LOG(DEBUG, YELLOW, it->first + " : ");
            else
                LOG(DEBUG, YELLOW, it->first + " : ", it->second);
        }
    }
	
}

void Post::saveInFile() const {
    // this->print();
    for (size_t i = 0; i < _header.size(); ++i) {
        std::map<std::string, std::string>::const_iterator it = _header[i].find("content-disposition");
        if (it == _header[i].end()) {
            throw Tools::Exception(400, "Post: No content disposition");
        }
        std::string filename;
        const std::string& name = it->second;
        size_t namePos = name.find("name=");
        if (namePos == std::string::npos) {
            throw Tools::Exception(400, "Post: No Name in content-disposition");
            // filename = "post";
        } else {
            std::size_t start = std::string::npos;
            std::size_t end = std::string::npos;
            if (name[namePos + 5] == '\"') {
                // std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAA" << name[namePos + 5] << std::endl;
                start = namePos + 6;
                end = name.find("\"", start);
            } else if (name[namePos + 5] != '\"') {
                // std::cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBB" << name[namePos + 5] << std::endl;
                start = namePos + 5;
                end = name.find_first_of(" \r\n", start);
            }
            if (end == std::string::npos) {
                throw Tools::Exception(400, "Post: No quote, whatever that means");
                // filename = "post";
            } else {
                filename = name.substr(start, end - start);
            }
        }
        it = _header[i].find("content-type");
        if (it != _header[i].end()) {
            //
            // const std::string& format = it->second;
            // size_t start = format.find("/");
            // std::string formatType = format.substr(start + 1);
            // size_t end = formatType.find_first_of(" \r\n\t;");
            //
            // if (end != std::string::npos) {
            //     formatType.erase(end);
            // }

            std::string contentType = it->second;
            size_t start = contentType.find_first_not_of(" \t\r\n");
			if (start != std::string::npos) {
				size_t end = contentType.find_first_of(";\r\n ", start);
                contentType = contentType.substr(start, end - start);
			}
            std::string formatType = HttpTools::getContentType(contentType);
            // std::clog << "formatType : " << formatType << " contentType : " << contentType << std::endl;
            filename += formatType;
        }
        std::ofstream outFile(("uploads/" + filename).c_str(), std::ios::out | std::ios::binary);
        if (!outFile) {
            throw Tools::Exception(500, "Post: Can't create file");
        }
        it = _header[i].find("body");
        outFile << it->second;
        outFile.close();
    }
}

const std::string Post::executePost(const HttpRequest &request)
{
    Post post(request);
    post.parseBody();
    post.saveInFile();

    HttpResponse response(HttpTools::getReturnPair(201));
	for (std::map<std::string, std::string>::const_iterator it = request.getHeader().begin(); it != request.getHeader().end(); ++it) {
		if (it->first == "content-length" || it->first == "content-type" || it->first == "connection"
			|| it->first == "server" || it->first == "cache-control" || it->first == "cookie") {
				if (it->first == "cookie") {
					response.addHeader("set-cookie", it->second);
				} else {
					response.addHeader(it->first, it->second);
				}
			}
	}
	response.addDateHeader();
	response.setBody(request.getBody());
    // LOG(DEBUG, PINK, response.getFinalResponse());
    return response.getFinalResponse();
}
