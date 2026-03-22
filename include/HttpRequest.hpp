#ifndef HTTPREQUEST_CLASS_HPP
#define HTTPREQUEST_CLASS_HPP

#include <set>
#include <map>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Tools.hpp"
#include "ServerSocket.hpp"
#include "HttpTools.hpp"

#define URI_MAX_LENGTH // To be defined if any.

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
	std::string _methodStr;
    std::string _path;
    std::string _purePath;
    std::map<std::string, std::string> _queryParams;
    std::string _httpVersion;
    std::map<std::string, std::string> _header;
    std::string _boundary;
    std::string _body;
	bool _headersParsed;

public:
	HttpRequest();
	HttpRequest(const std::string &request);
	HttpRequest(const HttpRequest &obj);
	HttpRequest &operator=(const HttpRequest &obj);
	~HttpRequest();

	const std::string &getMethodStr() const { return _methodStr;}
    const std::string &getPath() const {return _path;}
    const std::string &getPurePath() const {return _purePath;}
    const std::map<std::string, std::string> &getQueryParams() const {return _queryParams;}
    const std::string &getHttpVersion() const {return _httpVersion;}
    const std::map<std::string, std::string> &getHeader() const {return _header;}
    const std::string &getBoundary() const {return _boundary;}
    const std::string &getBody() const {return _body;}
	bool getHeadersParsed() const { return _headersParsed; }

	bool hasBody();
	bool parse(const std::string &request);
};

#endif
