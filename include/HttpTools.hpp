#ifndef HTTPTOOLS_HPP
#define HTTPTOOLS_HPP

#include "TerminalColors.hpp"
#include "Tools.hpp"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <set>

#define DEFAULT_PORT 8080
#define DEFAULT_PATH "/" // ROOT
#define SPACE " "
#define CR "\r"     // Carriage return
#define LF "\n"     // New line / linefeed
#define CRLF "\r\n" // Use to separate blocs in HTTP messages (headers, body...)

#define CONTENT_TYPE "Content-Type"
#define CONTENT_LENGTH "Content-Length"
#define STATUS "Status"

#define CONTENT_TYPE_PAIRS                                                                  \
    X(".aac", "audio/aac")                                                                  \
    X(".abw", "application/x-abiword")                                                      \
    X(".apng", "image/apng")                                                                \
    X(".arc", "application/x-freearc")                                                      \
    X(".avif", "image/avif")                                                                \
    X(".avi", "video/x-msvideo")                                                            \
    X(".azw", "application/vnd.amazon.ebook")                                               \
    X(".bin", "application/octet-stream")                                                   \
    X(".bmp", "image/bmp")                                                                  \
    X(".bz", "application/x-bzip")                                                          \
    X(".bz2", "application/x-bzip2")                                                        \
    X(".cda", "application/x-cdf")                                                          \
    X(".csh", "application/x-csh")                                                          \
    X(".css", "text/css")                                                                   \
    X(".csv", "text/csv")                                                                   \
    X(".doc", "application/msword")                                                         \
    X(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document")   \
    X(".eot", "application/vnd.ms-fontobject")                                              \
    X(".epub", "application/epub+zip")                                                      \
    X(".gz", "application/gzip")                                                            \
    X(".gif", "image/gif")                                                                  \
    X(".htm", "text/html")                                                                  \
    X(".html", "text/html")                                                                 \
    X(".ico", "image/vnd.microsoft.icon")                                                   \
    X(".ics", "text/calendar")                                                              \
    X(".jar", "application/java-archive")                                                   \
    X(".jpg", "image/jpeg")                                                                 \
    X(".js", "text/javascript")                                                             \
    X(".json", "application/json")                                                          \
    X(".jsonld", "application/ld+json")                                                     \
    X(".mid", "audio/midi")                                                                 \
    X(".midi", "audio/midi")                                                                \
    X(".mjs", "text/javascript")                                                            \
    X(".mp3", "audio/mpeg")                                                                 \
    X(".mp4", "video/mp4")                                                                  \
    X(".mpeg", "video/mpeg")                                                                \
    X(".mpkg", "application/vnd.apple.installer+xml")                                       \
    X(".odp", "application/vnd.oasis.opendocument.presentation")                            \
    X(".ods", "application/vnd.oasis.opendocument.spreadsheet")                             \
    X(".odt", "application/vnd.oasis.opendocument.text")                                    \
    X(".oga", "audio/ogg")                                                                  \
    X(".ogv", "video/ogg")                                                                  \
    X(".ogx", "application/ogg")                                                            \
    X(".opus", "audio/opus")                                                                \
    X(".otf", "font/otf")                                                                   \
    X(".png", "image/png")                                                                  \
    X(".pdf", "application/pdf")                                                            \
    X(".php", "application/x-httpd-php")                                                    \
    X(".ppt", "application/vnd.ms-powerpoint")                                              \
    X(".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation") \
    X(".rar", "application/vnd.rar")                                                        \
    X(".rtf", "application/rtf")                                                            \
    X(".sh", "application/x-sh")                                                            \
    X(".svg", "image/svg+xml")                                                              \
    X(".tar", "application/x-tar")                                                          \
    X(".tif", "image/tiff")                                                                 \
    X(".tiff", "image/tiff")                                                                \
    X(".ts", "video/mp2t")                                                                  \
    X(".ttf", "font/ttf")                                                                   \
    X(".txt", "text/plain")                                                                 \
    X(".vsd", "application/vnd.visio")                                                      \
    X(".wav", "audio/wav")                                                                  \
    X(".weba", "audio/webm")                                                                \
    X(".webm", "video/webm")                                                                \
    X(".webp", "image/webp")                                                                \
    X(".woff", "font/woff")                                                                 \
    X(".woff2", "font/woff2")                                                               \
    X(".xhtml", "application/xhtml+xml")                                                    \
    X(".xls", "application/vnd.ms-excel")                                                   \
    X(".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet")         \
    X(".xml", "application/xml")                                                            \
    X(".xul", "application/vnd.mozilla.xul+xml")                                            \
    X(".zip", "application/zip")                                                            \
    X(".3gp", "video/3gpp")                                                                 \
    X(".3g2", "video/3gpp2")                                                                \
    X(".7z", "application/x-7z-compressed")

#define PERCENT_ENCODING \
    X("%3A", ':')        \
    X("%2F", '/')        \
    X("%3F", '?')        \
    X("%23", '#')        \
    X("%5B", '[')        \
    X("%5D", ']')        \
    X("%40", '@')        \
    X("%21", '!')        \
    X("%24", '$')        \
    X("%26", '&')        \
    X("%27", '\'')       \
    X("%28", '(')        \
    X("%29", ')')        \
    X("%2A", '*')        \
    X("%2B", '+')        \
    X("%2C", ',')        \
    X("%3B", ';')        \
    X("%3D", '=')        \
    X("%25", '%')        \
    X("%20", ' ')        \
    X("+", ' ')

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

    static const std::string getContentType(const std::string &target);

    static const std::pair<int, std::string> getReturnPair(int code);
    static const MapType &getHttpCodes();
    static const std::string &getHttpReturnMessage(int code);
    static bool isValidHttpCode(int code);

    static const std::set<std::string> &getMethods();
    static bool isValidMethod(const std::string &method);

    static const std::set<std::string> &getHttpRequestHeaders();
    static bool isValidHttpRequestHeader(const std::string &header);

    static const std::vector<std::pair<std::string, std::string> > &getDecodedCharVec();

private:
    static void initPercentEncoding(std::vector<std::pair<std::string, std::string> > &list);
    static void initContentType(std::map<std::string, std::string> &contentType, bool isKey);
    static void initHttpCodes(HttpTools::MapType &httpCodes);
    static void initMethods(std::set<std::string> &methods);
    static void initHttpRequestHeaders(std::set<std::string> &httpHeaders);
};

#endif