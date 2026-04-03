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
CGI::CGI(const HttpRequest &request, const ConfigBase *config) : _request(request),
																 _config(config),
																 _cgiBinPath(config->getCGIPaths()._scriptFolderPath)
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
	if ((_pid = fork()) == -1)
		throw Tools::Exception(500, "CGI: Failed to create fork");
}

void CGI::setPostPipe()
{
	LOG(DEBUG, "CGI: POST");
	if (pipe(_postPipesFDs) == -1)
		throw Tools::Exception(500, "CGI: Failed to create pipe in POST");
	// ADD TO EPOLL
}

void CGI::setPostDup()
{
	Tools::closeAndResetFD(_postPipesFDs[1]);
	if (dup2(_pipeFDs[0], STDIN_FILENO) < 0)
		throw Tools::Exception(500, "CGI: Failed to dup2 in post");
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

char **CGI::buildParam(std::string &fullScriptPath)
{
	char *result[2];
	std::memset(result[0], '\0', _executablePath.size() + 1);
	std::memset(result[1], '\0', fullScriptPath.size() + 1);

	std::vector<char> temp(_executablePath.begin(), _executablePath.end());
	temp.push_back('\0');
	result[0] = &temp[0];

	temp.assign(fullScriptPath.begin(), fullScriptPath.end());
	temp.push_back('\0');
	result[1] = &temp[0];

	return result;
}

char **CGI::buildEnv() {
	
}

std::string CGI::checkAndExtractScript()
{
	const std::string &exec = _request.getPurePath();
	const std::string &scriptName = Tools::getStringAfterLastCharacter(exec, '/');
	const std::string &extension = Tools::getExtension(scriptName);
	if (extension != ".py" || extension != ".php")
		throw Tools::Exception(403, "Script extension is forbidden");

	const std::string fullPath = _config->getRoot() + exec;
	if (!Tools::isDirectory(fullPath.c_str()))
		throw Tools::Exception(403, "File path is a directory");
	if (!Tools::fileExists(fullPath.c_str()))
		throw Tools::Exception(404, "Script not found");
	if (!Tools::isExecutable(fullPath.c_str()))
		throw Tools::Exception(403, "Permission mismatch");

	if (extension == ".py")
		_executablePath = _config->getCGIPaths()._pythonPath;
	else if (extension == ".php")
		_executablePath = _config->getCGIPaths()._phpPath;

	return fullPath;
}

void CGI::executeCGI()
{
	LOG(DEBUG, "SCRIPT");
	std::string output;
	std::string fullScriptPath = checkAndExtractScript();
	const std::string &exec = _request.getPurePath();

	pipeAndFork();
	if (_request.getMethod() == "POST")
		setPostPipe();
	if (_pid == 0)
	{
		LOG(DEBUG, "CHILD");
		setChildPipe();
		if (_request.getMethod() == "POST")
			setPostDup();
		char **param = buildParam(fullScriptPath);
		char **env = buildEnv();
		execve(_pythonPath.c_str(), param, env);
		throw Tools::Exception(42, "CGI: child failed to execute");
	}
	else
	{
		LOG(DEBUG, "PARENT");
		Tools::closeAndResetFD(_pipeFDs[1]);
	}
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
	if (!Tools::fileExists(_request.getPurePath().c_str()))
		throw Tools::Exception(404, "CGI: file not found");
}

/**
 * @brief Parse the CGI output and give a formatted HttpResponse.
 * @return formatted and ready to send HttpResponse.
 */
const std::string CGI::getResponse()
{

}