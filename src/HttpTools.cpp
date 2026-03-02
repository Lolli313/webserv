#include "HttpTools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
HttpTools::HttpTools() {};

HttpTools::~HttpTools() {};

HttpTools::HttpTools(const HttpTools &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
HttpTools &HttpTools::operator=(const HttpTools &obj)
{
	if (this != &obj)
	{
	}
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

/*
=================================================================
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

const HttpTools::MapType& HttpTools::getHttpCodes() {
	static HttpTools::MapType httpCodes;
	if (httpCodes.empty())
		initHttpCodes(httpCodes);
	return httpCodes;
}

bool HttpTools::isValidHttpCode(int code) {
	const HttpTools::MapType& temp = getHttpCodes();
	return !(temp.find(code) == temp.end());
}

const std::string& HttpTools::getHttpReturnMessage(int code) {
    const HttpTools::MapType& temp = getHttpCodes();
    HttpTools::MapType::const_iterator it = temp.find(code);
    return ((it == temp.end()) ? temp.find(0)->second : it->second);
}


void HttpTools::initHttpCodes(HttpTools::MapType& httpCodes) {
    // If code not found
    httpCodes[0] = "";

    // 1xx Informational
    httpCodes[100] = "Continue";
    httpCodes[101] = "Switching Protocols";
    httpCodes[102] = "Processing";
    httpCodes[103] = "Early Hints";

    // 2xx Success
    httpCodes[200] = "OK";
    httpCodes[201] = "Created";
    httpCodes[202] = "Accepted";
    httpCodes[203] = "Non-Authoritative Information";
    httpCodes[204] = "No Content";
    httpCodes[205] = "Reset Content";
    httpCodes[206] = "Partial Content";
    httpCodes[207] = "Multi-Status";
    httpCodes[208] = "Already Reported";
    httpCodes[226] = "IM Used";

    // 3xx Redirection
    httpCodes[300] = "Multiple Choices";
    httpCodes[301] = "Moved Permanently";
    httpCodes[302] = "Found";
    httpCodes[303] = "See Other";
    httpCodes[304] = "Not Modified";
    httpCodes[305] = "Use Proxy";
    httpCodes[307] = "Temporary Redirect";
    httpCodes[308] = "Permanent Redirect";

    // 4xx Client Error
    httpCodes[400] = "Bad Request";
    httpCodes[401] = "Unauthorized";
    httpCodes[402] = "Payment Required";
    httpCodes[403] = "Forbidden";
    httpCodes[404] = "Not Found";
    httpCodes[405] = "Method Not Allowed";
    httpCodes[406] = "Not Acceptable";
    httpCodes[407] = "Proxy Authentication Required";
    httpCodes[408] = "Request Timeout";
    httpCodes[409] = "Conflict";
    httpCodes[410] = "Gone";
    httpCodes[411] = "Length Required";
    httpCodes[412] = "Precondition Failed";
    httpCodes[413] = "Payload Too Large";
    httpCodes[414] = "URI Too Long";
    httpCodes[415] = "Unsupported Media Type";
    httpCodes[416] = "Range Not Satisfiable";
    httpCodes[417] = "Expectation Failed";
    httpCodes[418] = "I'm a teapot";
    httpCodes[421] = "Misdirected Request";
    httpCodes[422] = "Unprocessable Entity";
    httpCodes[423] = "Locked";
    httpCodes[424] = "Failed Dependency";
    httpCodes[425] = "Too Early";
    httpCodes[426] = "Upgrade Required";
    httpCodes[428] = "Precondition Required";
    httpCodes[429] = "Too Many Requests";
    httpCodes[431] = "Request Header Fields Too Large";
    httpCodes[451] = "Unavailable For Legal Reasons";

    // 5xx Server Error
    httpCodes[500] = "Internal Server Error";
    httpCodes[501] = "Not Implemented";
    httpCodes[502] = "Bad Gateway";
    httpCodes[503] = "Service Unavailable";
    httpCodes[504] = "Gateway Timeout";
    httpCodes[505] = "HTTP Version Not Supported";
    httpCodes[506] = "Variant Also Negotiates";
    httpCodes[507] = "Insufficient Storage";
    httpCodes[508] = "Loop Detected";
    httpCodes[510] = "Not Extended";
    httpCodes[511] = "Network Authentication Required";
}

const std::set<std::string>& HttpTools::getMethods() {
    static std::set<std::string> methods;
    if (methods.empty())
        initMethods(methods);
    return methods;
}

bool HttpTools::isValidMethod(const std::string& method) {
    const std::set<std::string> temp = getMethods();
    return !(temp.find(method) == temp.end());
}

void HttpTools::initMethods(std::set<std::string>& methods) {
    methods.insert("OPTIONS");
    methods.insert("GET");
    methods.insert("HEAD");
    methods.insert("POST");
    methods.insert("PUT");
    methods.insert("DELETE");
    methods.insert("TRACE");
    methods.insert("CONNECT");
}

const std::set<std::string>& HttpTools::getHttpRequestHeaders() {
    static std::set<std::string> httpRequestHeaders;
    if (httpRequestHeaders.empty())
        initHttpRequestHeaders(httpRequestHeaders);
    return httpRequestHeaders;
}

bool HttpTools::isValidHttpRequestHeader(const std::string& header) {
    const std::set<std::string> temp = getHttpRequestHeaders();
    return !(temp.find(header) == temp.end());
}

void HttpTools::initHttpRequestHeaders(std::set<std::string>& httpRequestHeaders) {
    httpRequestHeaders.insert("Accept");
    httpRequestHeaders.insert("Accept-Charset");
    httpRequestHeaders.insert("Accept-Encoding");
    httpRequestHeaders.insert("Accept-Language");
    httpRequestHeaders.insert("Authorization");
    httpRequestHeaders.insert("Expect");
    httpRequestHeaders.insert("From");
    httpRequestHeaders.insert("Host");
    httpRequestHeaders.insert("If-Match");
    httpRequestHeaders.insert("If-Modified");
    httpRequestHeaders.insert("If-None-Match");
    httpRequestHeaders.insert("If-None-Range");
    httpRequestHeaders.insert("If-Unmodified-Since");
    httpRequestHeaders.insert("Max-Forwards");
    httpRequestHeaders.insert("Proxy-Authorization");
    httpRequestHeaders.insert("Range");
    httpRequestHeaders.insert("Referer");
    httpRequestHeaders.insert("TE");
    httpRequestHeaders.insert("User-Agent");
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

