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
CGI::CGI(const HttpRequest &request, const ConfigBase *config) : _cgiBinPath(config->getCGIPaths()._scriptFolderPath),
																 _pythonPath(config->getCGIPaths()._pythonPath),
																 _phpPath(config->getCGIPaths()._phpPath)
{
	try
	{
		setCGI(request, config);
		executeCGI(request, config);
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

CGI::CGI(const CGI &obj) { *this = obj; }

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
const std::string &CGI::getPythonPath() const { return _pythonPath; }
const std::string &CGI::getPhpPath() const { return _phpPath; }

void CGI::setPath(const std::string &src) { _cgiBinPath = src; }
void CGI::setPythonPath(const std::string &src) { _pythonPath = src; }
void CGI::setPhpPath(const std::string &src) { _phpPath = src; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

/**
 * @brief set the pipe of the CGI and fork.
 * @return the pid fork return value, to know if we are in the child or parent.
 */
void CGI::pipeAndFork()
{
	if (pipe(_pipeFDs) == -1)
		throw Tools::Exception(500, "CGI: Failed to create pipe");
	if ((_pid = fork()) == -1)
		throw Tools::Exception(500, "CGI: Failed to create pipe");
}

void CGI::handlePostCGI()
{
	LOG(DEBUG, "CGI: POST");
	if (pipe(_postPipesFDs) == -1)
		throw Tools::Exception(500, "CGI: Failed to create pipe in POST");
	Tools::closeAndResetFD(_postPipesFDs[1]);
	if (dup2(_postPipesFDs[0], STDIN_FILENO) < 0)
		throw Tools::Exception(500, "CGI: Failed to dup2");
	// SHOULD WE CLOSE IT ???? I DON'T THINK SO, WE NEED TO USE IT IN execute() TO ADD IT TO EPOLL
	Tools::closeAndResetFD(_postPipesFDs[0]);
}

void CGI::executeCGI(const HttpRequest &request, const ConfigBase *config)
{
	LOG(DEBUG, "SCRIPT");
	std::string output;
	std::string exec = request.getPurePath();

	pipeAndFork();
	if (_pid == 0)
	{
		LOG(DEBUG, "CHILD");
		Tools::closeAndResetFD(_pipeFDs[0]);
		if (dup2(_pipeFDs[1], STDOUT_FILENO) < 0)
			throw Tools::Exception(500, "CGI: Failed to dup2");
		Tools::closeAndResetFD(_pipeFDs[1]);

		if (request.getMethod() == "POST") {
			handlePostCGI();
		}
		char **param = buildParam();
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
bool CGI::handleCGI()
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
	return false;
	// le parent lit le pipe out a chaque fois et remplit le buffer
	// si il lit EOF a la fin
	// on remplit le buffer une derniere fois
	// on construit la reponse et la return

	// Event can be the read or write part of the pipe
	// It need to read from the pipe (the child is writing from the other side), and append this to the CGI _buffer
	// Then, once it has been determined that the CGI is finished, it will set a response and call handleResponse()
}

void CGI::setCGI(const HttpRequest &request, const ConfigBase *config)
{
	if (!Tools::fileExists(request.getPurePath().c_str()))
		throw Tools::Exception(404, "CGI: file not found");
}