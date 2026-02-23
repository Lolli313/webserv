#ifndef HTTPREQUEST_CLASS_HPP
#define HTTPREQUEST_CLASS_HPP

#include <set>
#include <map>
#include <string>
#include <vector>

#define URI_MAX_LENGTH // To be defined if any.
#define DEFAULT_PORT 80
#define DEFAULT_PATH "/" // ROOT
#define SPACE " "
#define CR "\r"     // Carriage return
#define LF "\n"     // New line / linefeed
#define CRLF "\r\n" // Use to separate blocs in HTTP messages (headers, body...)

enum httpMethods
{
  OPTION,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT
};

// All valid HTTP1.1 request headers.
class HttpRequestHeaders
{
public:
  static const std::set<std::string> &get()
  {
    static std::set<std::string> headers;
    if (headers.empty())
    {
      headers.insert("Accept");
      headers.insert("Accept-Charset");
      headers.insert("Accept-Encoding");
      headers.insert("Accept-Language");
      headers.insert("Authorization");
      headers.insert("Expect");
      headers.insert("From");
      headers.insert("Host");
      headers.insert("If-Match");
      headers.insert("If-Modified");
      headers.insert("If-None-Match");
      headers.insert("If-None-Range");
      headers.insert("If-Unmodified-Since");
      headers.insert("Max-Forwards");
      headers.insert("Proxy-Authorization");
      headers.insert("Range");
      headers.insert("Referer");
      headers.insert("TE");
      headers.insert("User-Agent");
    }
    return headers;
  }
};

class HttpRequest
{
private:
  // METHOD (GET / POST / PUT / DELETE...)
  httpMethods _method;

  // HTTP VERSION
  std::string _httpVersion;
  int _httpVersionMinor;
  int _httpVersionMajor;

  // UNIFORM RESSOURCE IDENTIFIERS (URI)
  int _port;
  std::string _host;
  std::string _absPath;
  std::string _query;
  std::string _ressource;
  std::string _requestURI; // Request line

  // HEADERS
  std::map<std::string, std::string> _headersMap;

  // BODY
  std::string _body;

  HttpRequest();

public:
  HttpRequest(const std::string &request);
  HttpRequest(const HttpRequest &obj);
  HttpRequest &operator=(const HttpRequest &obj);
  ~HttpRequest();

  bool hasBody();
  std::string::iterator &whileSpace(std::string::iterator &); // Iterate while SPACE, but returns error if no space
  std::vector<std::string> splitString(const std::string &str, const std::string &separator);
};

#endif
