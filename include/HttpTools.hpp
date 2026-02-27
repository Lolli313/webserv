#ifndef HTTPTOOLS_HPP
#define HTTPTOOLS_HPP

#include <string>
#include <map>

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

class HttpTools
{
public:
    typedef std::map<int, std::string> MapType;

    HttpTools();
    HttpTools(const HttpTools &obj);
    HttpTools &operator=(const HttpTools &obj);
    ~HttpTools();

    static const MapType& getMap();
    static bool isValidHttpCode(int code);

private:
    static void initHttpCodes(HttpTools::MapType& httpCodes);
};

#endif