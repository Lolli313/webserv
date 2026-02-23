#ifndef HTTPTOOLS_HPP
#define HTTPTOOLS_HPP

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
	HttpTools();
	HttpTools(const HttpTools &obj);
	HttpTools &operator=(const HttpTools &obj);
	~HttpTools();

private:
};

#endif