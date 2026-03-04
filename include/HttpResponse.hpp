#pragma once

#include <string>
#include <map>
#include "HttpTools.hpp"
#include "Tools.hpp"

#define HTTP_VERSION "HTTP/1.1"

/**
 * @brief A HTTP Response class. Build it with the 2 different constructors to get a minimal usable version.
 * @return Call getFinalResponse to build and get the HTTP Response as a std::string (_finalResponse)
 * @attention quickHttpResponse function available to get a HTTP Response in one call without having to instantiate a HttpResponse object.
 */
class HttpResponse
{
private:
	// HTTP VERSION
	std::string _httpVersion;

	// RETURN CODE & MESSAGE
	int _returnCode;
	std::string _returnMessage;

	// HEADERS
	std::map<std::string, std::string> _reponseHeaders;

	// BODY
	std::string _body;

	// FINAL RESPONSE TO BE STRANSFERED TO THE CLIENT
	// Is setted by the getFinalResponse call
	std::string _finalResponse;
	HttpResponse();

public:
	HttpResponse(int code, const std::string &message);
	HttpResponse(const std::string &httpVersion, int code, const std::string &message);
	HttpResponse(const HttpResponse &obj);
	HttpResponse &operator=(const HttpResponse &obj);
	~HttpResponse();

	void setHttpVersion(const std::string &httpVersion);
	void setReturnCode(int code);
	void setReturnMessage(const std::string &returnMessage);
	void setResponseHeaders(const std::map<std::string, std::string> &responseHeaders);
	void setBody(const std::string &body);

	const std::string &getHttpVersion() const;
	int getReturnCode() const;
	const std::string &getReturnMessage() const;
	const std::map<std::string, std::string> &getResponseHeaders() const;
	const std::map<std::string, std::string> &getRepresentationHeaders() const;
	const std::string &getBody() const;
	const std::string &getFinalResponse(); // This call builds and return the final response.

	void addHeadersToResponse();
	void buildFinalResponse();
};

const std::string quickHttpReponse(int code, const std::string &message);
const std::string quickHttpReponse(std::pair<int, const std::string &> &response);