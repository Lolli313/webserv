#include "HttpTools.hpp"
#include <ctime>
#include <cstdio>

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
    (void)obj;
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

/**
 * @brief Takes a content-type or an extension and returns its counterpart. (ex: extension=.txt, content-type=text/plain)
 */
const std::string HttpTools::getContentType(const std::string &target)
{
    static std::map<std::string, std::string> mapKeyValue;
    static std::map<std::string, std::string> mapValueKey;
    std::map<std::string, std::string>::const_iterator it;
    if (mapKeyValue.empty() || mapValueKey.empty())
    {
        initContentType(mapKeyValue, true);
        initContentType(mapValueKey, false);
    }

    it = mapKeyValue.find(target);
    if (it != mapKeyValue.end())
        return it->second;
    
    it = mapValueKey.find(target);
    if (it != mapValueKey.end())
        return it->second;
    return std::string("");
}

/**
 * @param isKey if true, takes the key as key, and if false, it uses the key as value.
 */
void HttpTools::initContentType(std::map<std::string, std::string> &contentType, bool isKey) {
    if (isKey) {
        #define X(key, value) contentType[key] = value;
            CONTENT_TYPE_PAIRS
        #undef X
    }
    else {
        #define X(key, value) contentType[value] = key;
            CONTENT_TYPE_PAIRS
        #undef X
    }
}

void HttpTools::initPercentEncoding(std::vector<std::pair<std::string, std::string> > &list)
{
    #define X(key, value) list.push_back(std::make_pair(std::string(key), std::string(1, value)));
        PERCENT_ENCODING
    #undef X
} 

const std::vector<std::pair<std::string, std::string> > &HttpTools::getDecodedCharVec()
{
    static std::vector<std::pair<std::string, std::string> > list;
    if (list.empty())
        initPercentEncoding(list);
    return list; 
}

const HttpTools::MapType &HttpTools::getHttpCodes()
{
    static HttpTools::MapType httpCodes;
    if (httpCodes.empty())
        initHttpCodes(httpCodes);
    return httpCodes;
}

bool HttpTools::isValidHttpCode(int code)
{
    const HttpTools::MapType &temp = getHttpCodes();
    return !(temp.find(code) == temp.end());
}

const std::string &HttpTools::getHttpReturnMessage(int code)
{
    const HttpTools::MapType &temp = getHttpCodes();
    HttpTools::MapType::const_iterator it = temp.find(code);
    return ((it == temp.end()) ? temp.find(0)->second : it->second);
}

/** @return A pair<int, const std::string &> with the Http Return Code and the Http Reason-Phrase
 **/
const std::pair<int, std::string> HttpTools::getReturnPair(int code)
{
    const HttpTools::MapType &temp = getHttpCodes();
    HttpTools::MapType::const_iterator it = temp.find(code);
    if (it == temp.end())
        it = temp.find(0);

    return std::pair<int, std::string>(it->first, it->second);
}

void HttpTools::initHttpCodes(HttpTools::MapType &httpCodes)
{
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

const std::set<std::string> &HttpTools::getMethods()
{
    static std::set<std::string> methods;
    if (methods.empty())
        initMethods(methods);
    return methods;
}

bool HttpTools::isValidMethod(const std::string &method)
{
    const std::set<std::string> temp = getMethods();
    return !(temp.find(method) == temp.end());
}

void HttpTools::initMethods(std::set<std::string> &methods)
{
    methods.insert("OPTIONS");
    methods.insert("GET");
    methods.insert("HEAD");
    methods.insert("POST");
    methods.insert("PUT");
    methods.insert("DELETE");
    methods.insert("TRACE");
    methods.insert("CONNECT");
}

const std::set<std::string> &HttpTools::getHttpRequestHeaders()
{
    static std::set<std::string> httpRequestHeaders;
    if (httpRequestHeaders.empty())
        initHttpRequestHeaders(httpRequestHeaders);
    return httpRequestHeaders;
}

bool HttpTools::isValidHttpRequestHeader(const std::string &header)
{
    const std::set<std::string> temp = getHttpRequestHeaders();
    return !(temp.find(header) == temp.end());
}

void HttpTools::initHttpRequestHeaders(std::set<std::string> &httpRequestHeaders) {
    // En-têtes de base
    httpRequestHeaders.insert("accept");
    httpRequestHeaders.insert("accept-charset");
    httpRequestHeaders.insert("accept-encoding");
    httpRequestHeaders.insert("accept-language");
    httpRequestHeaders.insert("authorization");
    httpRequestHeaders.insert("cache-control");
    httpRequestHeaders.insert("connection");
    httpRequestHeaders.insert("content-length");
    httpRequestHeaders.insert("content-type");
    httpRequestHeaders.insert("cookie");
    httpRequestHeaders.insert("host");
    httpRequestHeaders.insert("origin");
    httpRequestHeaders.insert("referer");
    httpRequestHeaders.insert("user-agent");

    // En-têtes conditionnels
    httpRequestHeaders.insert("if-match");
    httpRequestHeaders.insert("if-modified-since");
    httpRequestHeaders.insert("if-none-match");
    httpRequestHeaders.insert("if-range");
    httpRequestHeaders.insert("if-unmodified-since");
    httpRequestHeaders.insert("pragma");

    // En-têtes pour les requêtes multiparties et plages
    httpRequestHeaders.insert("content-disposition");
    httpRequestHeaders.insert("range");
    httpRequestHeaders.insert("te");

    // En-têtes de sécurité
    httpRequestHeaders.insert("content-security-policy");
    httpRequestHeaders.insert("dnt");
    httpRequestHeaders.insert("strict-transport-security");
    httpRequestHeaders.insert("upgrade-insecure-requests");
    httpRequestHeaders.insert("x-content-type-options");
    httpRequestHeaders.insert("x-frame-options");
    httpRequestHeaders.insert("x-xss-protection");

    // En-têtes pour les proxies et CDN
    httpRequestHeaders.insert("forwarded");
    httpRequestHeaders.insert("x-forwarded-for");
    httpRequestHeaders.insert("x-forwarded-host");
    httpRequestHeaders.insert("x-forwarded-proto");
    httpRequestHeaders.insert("x-real-ip");
    httpRequestHeaders.insert("via");

    // En-têtes pour les APIs et AJAX
    httpRequestHeaders.insert("access-control-request-method");
    httpRequestHeaders.insert("access-control-request-headers");
    httpRequestHeaders.insert("x-api-key");
    httpRequestHeaders.insert("x-requested-with");

    // En-têtes pour les performances et réseaux modernes
    httpRequestHeaders.insert("save-data");
    httpRequestHeaders.insert("sec-fetch-dest");
    httpRequestHeaders.insert("sec-fetch-mode");
    httpRequestHeaders.insert("sec-fetch-site");
    httpRequestHeaders.insert("sec-fetch-user");
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
