#pragma once

#include <string>
#include <map>

class HttpResponse
{
private:
	// HTTP VERSION
	std::string _httpVersion;
	// int _httpVersionMinor;
	// int _httpVersionMajor;

	// RETURN CODE & MESSAGE
	// int _returnCode;
	std::string _returnMessage;

	// HEADERS
	std::map<std::string, std::string> _reponseHeaders;
	std::map<std::string, std::string> _representationHeaders; // if the message has a body

	// BODY
	std::string _body;

	// FINAL RESPONSE TO BE STRANSFERED TO THE CLIENT
	std::string _finalResponse;

public:
	HttpResponse();
	HttpResponse(const HttpResponse &obj);
	HttpResponse &operator=(const HttpResponse &obj);
	~HttpResponse();
};
