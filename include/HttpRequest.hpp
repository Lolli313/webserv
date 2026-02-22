#ifndef HTTPREQUEST_CLASS_HPP
#define HTTPREQUEST_CLASS_HPP

#include <string>

#define URI_MAX_LENGTH // To be defined if any.
#define DEFAULT_PORT 80
#define DEFAULT_PATH "/" // ROOT
#define SPACE " "
#define CR "\r"     // Carriage return
#define LF "\n"     // New line / linefeed
#define CRLF "\r\n" // Use to separate blocs in HTTP messages (headers, body...)

enum HttpMethod { OPTION, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT };

class HttpRequest {
private:
  // HTTP VERSION
  std::string httpVersion;
  int httpVersionMinor;
  int httpVersionMajor;

  // UNIFORM RESSOURCE IDENTIFIERS (URI)
  std::string host;
  int port;
  std::string absPath;
  std::string query;

  // METHOD (GET / POST / PUT / DELETE...)
  HttpMethod method;

  // BODY
  std::string body;

  HttpRequest();

public:
  HttpRequest(const std::string &request);
  HttpRequest(const HttpRequest &obj);
  HttpRequest &operator=(const HttpRequest &obj);
  ~HttpRequest();
};

#endif
