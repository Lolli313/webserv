#pragma once

#include <string>
#include <map>

#define HTTP_VERSION "HTTP/1.1"

/**
 * @brief A HTTP Response class.
 * @details
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
	std::map<std::string, std::string> _representationHeaders; // if the message has a body

	// BODY
	std::string _body;

	// FINAL RESPONSE TO BE STRANSFERED TO THE CLIENT
	std::string _finalResponse;
	HttpResponse();

public:
	HttpResponse(int code, const std::string &message);
	HttpResponse(const std::string &httpVersion, int code, const std::string &message);
	HttpResponse(const HttpResponse &obj);
	HttpResponse &operator=(const HttpResponse &obj);
	~HttpResponse();

	void setResponseHeaders(std::map<std::string, std::string> responseHeaders);
	void setRepresentationHeaders(std::map<std::string, std::string> representationHeaders);

	const std::string &getFinalResponse() const; // This call builds and return the final response.
};

/**
 * @brief Get an HTTP Response formatted without having to create an object.
 * @param The return code and the Reason-Phrase (return message).
 * @return A const std::string formatted and ready to be sent to the client. With the default HTTP_VERSION, the return code and the return message.
 */
const std::string quickHttpReponse(int code, const std::string &message);

/**
 * @brief Get an HTTP Response formatted without having to create an object. Directly with a pair<code, message>
 * @param  pair<return code, message> : A ReturnPair (can be found in HttpTools{.hpp/.cpp}) of int (return code) and const std::string & (Reason Phrase / return message).
 * @return A const std::string formatted and ready to be sent to the client. With the default HTTP_VERSION, the return code and the return message.
 */
const std::string quickHttpReponse(std::pair<int, const std::string &> &response);