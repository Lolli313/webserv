#include "HttpResponse.hpp"

// void HttpResponse::buildFinalResponse();

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
HttpResponse::HttpResponse(int code, const std::string &message) : _httpVersion(HTTP_VERSION), _returnCode(code), _returnMessage(message) {}
HttpResponse::HttpResponse(const std::string &httpVersion, int code, const std::string &message) : _httpVersion(httpVersion), _returnCode(code), _returnMessage(message) {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse &obj) { *this = obj; }

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
HttpResponse &HttpResponse::operator=(const HttpResponse &obj)
{
	if (this != &obj)
	{
		this->_httpVersion = obj._httpVersion;
		this->_returnCode = obj._returnCode;
		this->_returnMessage = obj._returnMessage;
		this->_reponseHeaders = obj._reponseHeaders;
		this->_body = obj._body;
		this->_finalResponse = obj._finalResponse;
	}
	return (*this);
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

void HttpResponse::setHttpVersion(const std::string &httpVersion) { _httpVersion = httpVersion; }
void HttpResponse::setReturnCode(int code) { _returnCode = code; }
void HttpResponse::setReturnMessage(const std::string &returnMessage) { _returnMessage = returnMessage; }
void HttpResponse::setResponseHeaders(const std::map<std::string, std::string> &responseHeaders) { _reponseHeaders = responseHeaders; }
void HttpResponse::setBody(const std::string &body) { _body = body; }

const std::string &HttpResponse::getHttpVersion() const { return _httpVersion; }
int HttpResponse::getReturnCode() const { return _returnCode; }
const std::string &HttpResponse::HttpResponse::getReturnMessage() const { return _returnMessage; }
const std::map<std::string, std::string> &HttpResponse::getResponseHeaders() const { return _reponseHeaders; }
const std::string &HttpResponse::getBody() const { return _body; }

/**
 * @brief This call builds and return the final response.
 * @throw Throws an exception with code 0 and a message log if an error occur (ex: missing HttpVersion, returnCode or returnMessage)
 */
const std::string &HttpResponse::getFinalResponse()
{
	if (_finalResponse.empty())
		buildFinalResponse();
	return _finalResponse;
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

/**
 * @brief Add the formated headers to the _finalResponse, and add the CRLF to mark the end of headers
 */
void HttpResponse::addHeadersToResponse()
{
	for (std::map<std::string, std::string>::iterator it = _reponseHeaders.begin(); it != _reponseHeaders.end(); it++)
	{
		_finalResponse.append(it->first + ":" + SPACE + it->second + CRLF);
	}
	_finalResponse.append(CRLF);
}

/**
 * @brief This call builds the final response.
 * @throw Throws an exception with code 0 and a message log if an error occur (ex: missing HttpVersion, returnCode or returnMessage)
 */
void HttpResponse::buildFinalResponse()
{
	if (!_httpVersion.empty())
		_finalResponse.append(_httpVersion + SPACE);
	else
		throw Tools::Exception("HttpResponse: missing _httpVersion");

	if (_returnCode >= 100)
		_finalResponse.append(_returnCode + SPACE);
	else
		throw Tools::Exception("HttpResponse: wrong _returnCode");

	if (!_returnMessage.empty())
		_finalResponse.append(_returnMessage + CRLF);
	else
		throw Tools::Exception("HttpResponse: missing _returnMessage");

	if (!_reponseHeaders.empty())
		addHeadersToResponse();

	if (!_body.empty())
		_finalResponse.append(_body);
}
