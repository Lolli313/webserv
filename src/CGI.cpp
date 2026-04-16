#include "CGI.hpp"

#include <sys/wait.h>
#include <ctime>
#include <iomanip>
#include <sstream>

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
CGI::CGI(const HttpRequest &request, const ConfigBase *config) :
	_request(request),
	_config(config),
	_cgiBinPath(config->getCGIPaths()._scriptFolderPath),
	_responseStatus(200),
	_timestamp(std::time(0)),
	_postPos(0)
{
	try
	{
		setCGI();
		executeCGI();
	}
	catch (Tools::Exception &e)
	{
		Tools::closeAndResetFD(_postPipesFDs[0]);
		Tools::closeAndResetFD(_postPipesFDs[1]);
		Tools::closeAndResetFD(_pipeFDs[0]);
		Tools::closeAndResetFD(_pipeFDs[1]);
		throw;
	}
}

CGI::~CGI()
{
	LOG(INFO, RED_BRIGHT, "Calling CGI destructor");
	Tools::closeAndResetFD(_postPipesFDs[0]);
	Tools::closeAndResetFD(_postPipesFDs[1]);
	Tools::closeAndResetFD(_pipeFDs[0]);
	Tools::closeAndResetFD(_pipeFDs[1]);
}

CGI::CGI(const CGI &obj) : _request(obj._request), _config(obj._config) { *this = obj; }

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
CGI &CGI::operator=(const CGI &obj)
{
	if (this != &obj)
	{
	}
	return (*this);
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const std::string &CGI::getPath() const { return _cgiBinPath; }

const int &CGI::getPid() const { return _pid; }
const std::time_t &CGI::getTimeStamp() const { return _timestamp; }

void CGI::setPath(const std::string &src) { _cgiBinPath = src; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

/**
 * @brief set the pipe of the CGI and fork.
 */
void CGI::pipeAndFork()
{
	if (pipe(_pipeFDs) == -1)
		throw Tools::Exception(500, "CGI: Failed to create pipe");

	int flags = fcntl(_pipeFDs[0], F_GETFD);
	if (flags < 0)
		throw Tools::Exception("pipeAndFork: flags error");
	if (fcntl(_pipeFDs[0], F_SETFD, flags | FD_CLOEXEC) < 0)
		throw Tools::Exception("fcntl");

	flags = fcntl(_pipeFDs[1], F_GETFD);
	if (flags < 0)
		throw Tools::Exception("pipeAndFork: flags error");
	if (fcntl(_pipeFDs[1], F_SETFD, flags | FD_CLOEXEC) < 0)
		throw Tools::Exception("fcntl");

	if ((_pid = fork()) == -1)
		throw Tools::Exception(500, "CGI: Failed to create fork");
}

void CGI::setPostPipe()
{
	if (pipe(_postPipesFDs) == -1)
		throw Tools::Exception(500, "CGI: Failed to create pipe in POST");

	int flags = fcntl(_postPipesFDs[0], F_GETFD);
	if (flags < 0)
		throw Tools::Exception("pipeAndFork: flags error");
	if (fcntl(_postPipesFDs[0], F_SETFD, flags | FD_CLOEXEC) < 0)
		throw Tools::Exception("fcntl");

	flags = fcntl(_postPipesFDs[1], F_GETFD);
	if (flags < 0)
		throw Tools::Exception("pipeAndFork: flags error");
	if (fcntl(_postPipesFDs[1], F_SETFD, flags | FD_CLOEXEC) < 0)
		throw Tools::Exception("fcntl");

	// ADD TO EPOLL
}

void CGI::setPostDup()
{
	Tools::closeAndResetFD(_postPipesFDs[1]);
	if (dup2(_postPipesFDs[0], STDIN_FILENO) < 0)
		throw Tools::Exception(42, "CGI: Failed to dup2 in post");
}

/**
 * @brief Close the read end of the pipe, send the body that has not been yet sent, and once done close the write end of the pipe.
 * @return true when it's done sending the body, false when not.
 */
bool CGI::handlePostCGI()
{
	// Pass the body at the _postPos index
	ssize_t written = write(_postPipesFDs[1], _request.getBody().c_str() + _postPos, _request.getBody().size() - _postPos);
	if (written < 0)
	{
		Tools::closeAndResetFD(_postPipesFDs[1]);
		throw Tools::Exception(500, "CGI: Failed to write body to pipe");
	}
	_postPos += written;

	// Check if we are done writing
	if (_postPos >= _request.getBody().size())
	{
		Tools::closeAndResetFD(_postPipesFDs[1]);
		return true;
	}
	return false;
}

void CGI::setChildPipe()
{
	Tools::closeAndResetFD(_pipeFDs[0]);
	if (dup2(_pipeFDs[1], STDOUT_FILENO) < 0)
		throw Tools::Exception(500, "CGI: Failed to dup2");
	Tools::closeAndResetFD(_pipeFDs[1]);
}

/**
 * @brief Builds the arguments to be used later in execve
 * @param fullScriptPath Full absolute path to the script
 * @param param[2] The char* array to be filled with the arguments
 */
void CGI::buildParam(std::string &fullScriptPath, char *param[3])
{
	param[0] = const_cast<char *>(_executablePath.c_str());
	if (_executablePath == _config->getRoot() + "cgi-bin/a.out")
		param[1] = NULL;
	else
		param[1] = const_cast<char *>(fullScriptPath.c_str());
	param[2] = NULL;
}

const std::string buildMetaVariable(const std::string &key, const std::string &value)
{
	return key + "=" + value;
}

const std::string transformMapToQueryString(const std::map<std::string, std::string> &queryMap)
{
	std::map<std::string, std::string>::const_iterator it = queryMap.begin();
	std::string result;
	for (; it != queryMap.end(); it++)
	{
		if (!result.empty())
			result += "&";
		result += it->first + "=" + it->second;
	}
	return result;
}

/**
 * @brief Builds the environment to be used later in execve
 * @param env The char** reference to be filled with the environment for execve
 * @param envVector
 */
void CGI::buildEnv(char **&env, std::vector<std::string> &envVector)
{
	std::string temp;
	if (!_request.getBody().empty())
	{
		if ((temp = _request.findHeader("content-length")) == "")
			throw Tools::Exception(500, "No Content-Length header found unexpectedly");
		envVector.push_back(buildMetaVariable("CONTENT_LENGTH", temp));
		if ((temp = _request.findHeader("content-type")) == "")
			throw Tools::Exception(500, "No Content-Type header found unexpectedly");
		envVector.push_back(buildMetaVariable("CONTENT_TYPE", temp));
	}
	envVector.push_back(buildMetaVariable("GATEWAY_INTERFACE", "CGI/1.1"));
	envVector.push_back(buildMetaVariable("QUERY_STRING", transformMapToQueryString(_request.getQueryParams())));

	char *ip = inet_ntoa(_request.getClientAddr().sin_addr);
	std::string ipStr = (ip) ? ip : "127.0.0.1";
	envVector.push_back(buildMetaVariable("REMOTE_ADDR", ipStr));
	envVector.push_back(buildMetaVariable("REQUEST_METHOD", _request.getMethod()));
	envVector.push_back(buildMetaVariable("SERVER_NAME", _request.getServerName()));
	envVector.push_back(buildMetaVariable("SERVER_PORT", _request.getPort()));
	envVector.push_back(buildMetaVariable("SERVER_PROTOCOL", _request.getHttpVersion()));
	envVector.push_back(buildMetaVariable("SERVER_SOFTWARE", "webserv/1.1"));

	if (_executablePath == _config->getCGIPaths()._phpPath)
	{
		envVector.push_back(buildMetaVariable("REDIRECT_STATUS", "200"));
		envVector.push_back(buildMetaVariable("SCRIPT_FILENAME", _config->getRoot() + _request.getPurePath()));
	}

	env = new char *[envVector.size() + 1];
	for (std::size_t i = 0; i < envVector.size(); i++)
	{
		env[i] = const_cast<char *>(envVector[i].c_str());
	}
	env[envVector.size()] = NULL;
}

/**
 * @brief Checks whether the requested path is valid
 * @returns The full absolute path to the script
 */
std::string CGI::checkAndExtractScript()
{
	const std::string &exec = _request.getPurePath();
	const std::string &scriptName = Tools::getStringAfterLastCharacter(exec, '/');
	const std::string &extension = Tools::extractExtension(scriptName);

	const std::string fullPath = _config->getRoot() + exec;
	if (Tools::isDirectory(fullPath.c_str()))
		throw Tools::Exception(403, "File path is a directory");
	if (extension != ".py" && extension != ".php" && extension != ".c")
		throw Tools::Exception(403, "Invalid script extension");
	if (!Tools::fileExists(fullPath.c_str()))
		throw Tools::Exception(404, "Script not found");
	if (!Tools::isExecutable(fullPath.c_str()))
		throw Tools::Exception(403, "Permission mismatch");

	if (extension == ".py")
		_executablePath = _config->getCGIPaths()._pythonPath;
	else if (extension == ".php")
		_executablePath = _config->getCGIPaths()._phpPath;
	else if (extension == ".c")
		_executablePath = _config->getRoot() + "cgi-bin/a.out";

	return fullPath;
}

void CGI::executeCGI()
{
	std::string output;
	std::string fullScriptPath = checkAndExtractScript();
	const std::string &exec = _request.getPurePath();
	(void)exec;
	bool methodIsPost = false;
	if (_request.getMethod() == "POST")
		methodIsPost = true;

	if (methodIsPost)
		setPostPipe();
	pipeAndFork();
	if (_pid == 0)
	{
		g_isChild = true;
		setChildPipe();
		if (methodIsPost)
			setPostDup();

		char *param[3] = {0};
		buildParam(fullScriptPath, param);

		char **env;
		std::vector<std::string> envVector;
		buildEnv(env, envVector);

		execve(param[0], param, env);

		delete[] env;
		throw Tools::Exception(42, "CGI: child failed to execute");
	}
	else
		Tools::closeAndResetFD(_pipeFDs[1]);
}

/**
 * @brief Read the output of the CGI.
 * @return true when it reaches EOF, false otherwise.
 */
bool CGI::readCgiOutput()
{
	char buffer[BUFFERSIZE];
	std::memset(buffer, '\0', BUFFERSIZE);
	ssize_t bytesRead;
	bytesRead = read(_pipeFDs[0], buffer, BUFFERSIZE);
	if (bytesRead > 0)
	{
		_buffer.append(buffer);
		return false;
	}
	else if (bytesRead < 0)
	{
		// Should we destroy the CGI object ?
		Tools::closeAndResetFD(_pipeFDs[0]);
		Tools::Exception(500, "CGI: Error reading output");
		return false;
	}
	else
	{
		LOG(INFO, "Finished reading CGI output");
		Tools::closeAndResetFD(_pipeFDs[0]);
		return true;
	}
	return true;
	// le parent lit le pipe out a chaque fois et remplit le buffer
	// si il lit EOF a la fin
	// on remplit le buffer une derniere fois
	// on construit la reponse et la return

	// Event can be the read or write part of the pipe
	// It need to read from the pipe (the child is writing from the other side), and append this to the CGI _buffer
	// Then, once it has been determined that the CGI is finished, it will set a response and call handleResponse()
}

void CGI::setCGI()
{
	_pipeFDs[0] = -1;
	_pipeFDs[1] = -1;
	_postPipesFDs[0] = -1;
	_postPipesFDs[1] = -1;
	std::string fullPath = _config->getRoot() + _request.getPurePath();
	if (!Tools::fileExists(fullPath.c_str()))
		throw Tools::Exception(404, "CGI: file not found");
}

/**
 * @brief Extract the headers of the CGI output.
 */
std::vector<std::pair<std::string, std::string> > CGI::parseHeaders()
{
	std::vector<std::pair<std::string, std::string> > result;
	std::size_t headersEnd = _buffer.find(CRLF CRLF);

	// length of "\r\n\r\n"
	std::size_t separatorLen = 4;

	if (headersEnd == std::string::npos)
	{
		headersEnd = _buffer.rfind(LF LF);
		// length of "\n\n"
		separatorLen = 2;
	}
	if (headersEnd == std::string::npos)
		throw Tools::Exception(500, "CGI: Missing header-body separator");

	std::string headers = _buffer.substr(0, headersEnd);
	// Ignore the headers and the separator (\r\n\r\n or \n\n)
	std::string body = _buffer.substr(headersEnd + separatorLen);
	_buffer = body;

	std::stringstream ss(headers);
	std::string line;
	while (std::getline(ss, line))
	{
		if (!line.empty() && Tools::getLastCharacter(line) == '\r')
			Tools::removeLastCharacter(line);

		if (line.empty())
			continue;

		std::size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			if (!value.empty() && value[0] == ' ')
				value.erase(0, 1);

			result.push_back(std::make_pair(key, value));
		}
	}
	return result;
}

std::pair<std::string, std::string> *getPairFromHeaders(std::vector<std::pair<std::string, std::string> > &headers, const std::string &target)
{
	for (std::vector<std::pair<std::string, std::string> >::iterator it = headers.begin(); it != headers.end(); it++)
	{
		if (it->first == target)
			return &(*it);
	}
	return NULL;
}

void setStatus(const std::string &line, int &status, std::string &statusMessage)
{
	std::istringstream iss(line);
	std::string label;
	int code;

	iss >> code >> label;
	if (code < 100 || code > 999)
		throw Tools::Exception(500, "CGI: Response status code is invalid");
	status = code;
	if (!label.empty())
		statusMessage = label;
}

void removeHeader(std::vector<std::pair<std::string, std::string> > &headers, const std::string &key)
{
	for (std::vector<std::pair<std::string, std::string> >::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		if (it->first == key)
		{
			headers.erase(it);
			break;
		}
	}
}

/**
 * @brief Parse the CGI output and give a formatted HttpResponse.
 * @return formatted and ready to send HttpResponse.
 */
const std::string CGI::getResponse()
{
	std::pair<std::string, std::string> *oneHeader;
	std::vector<std::pair<std::string, std::string> > headers = parseHeaders();

	oneHeader = getPairFromHeaders(headers, CONTENT_TYPE);
	if (!oneHeader)
		throw Tools::Exception(502, "CGI: no Content-Type header");

	std::string responseStatusMessage;
	oneHeader = getPairFromHeaders(headers, STATUS);
	if (oneHeader)
	{
		setStatus(oneHeader->second, _responseStatus, responseStatusMessage);
		// erase the Status header since that shouldn't be sent back to the client
		removeHeader(headers, STATUS);
	}

	std::pair<int, std::string> httpPair = HttpTools::getReturnPair(_responseStatus);
	if (httpPair.first == 0)
		httpPair = std::make_pair(_responseStatus, responseStatusMessage);

	HttpResponse response(HttpTools::getReturnPair(_responseStatus));
	response.addDateHeader();
	removeHeader(headers, CONTENT_LENGTH);
	response.setResponseHeaders(headers);
	response.setBody(_buffer);
	response.addHeader(CONTENT_LENGTH, Tools::intToString(_buffer.size()));
	return response.getFinalResponse();
}
