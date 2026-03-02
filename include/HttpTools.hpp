#ifndef HTTPTOOLS_HPP
#define HTTPTOOLS_HPP

#include <string>
#include <map>
#include <set>

#define DEFAULT_PORT 80
#define DEFAULT_PATH "/" // ROOT
#define SPACE " "
#define CR "\r"     // Carriage return
#define LF "\n"     // New line / linefeed
#define CRLF "\r\n" // Use to separate blocs in HTTP messages (headers, body...)

enum httpMethods
{
  OPTIONS,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT
};

class HttpTools
{
public:
    typedef std::map<int, std::string> MapType;

    HttpTools();
    HttpTools(const HttpTools &obj);
    HttpTools &operator=(const HttpTools &obj);
    ~HttpTools();

    static const MapType& getHttpCodes();
    static const std::string& getHttpReturnMessage(int code);
    static bool isValidHttpCode(int code);

    static const std::set<std::string>& getMethods();
    static bool isValidMethod(const std::string& method);

    static const std::set<std::string>& getHttpRequestHeaders();
    static bool isValidHttpRequestHeader(const std::string& header);

private:
    static void initHttpCodes(HttpTools::MapType& httpCodes);
    static void initMethods(std::set<std::string>& methods);
    static void initHttpRequestHeaders(std::set<std::string>& httpHeaders);
};

#endif