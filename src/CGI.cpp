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
CGI::CGI(const std::string &path, const std::string &pythonPath, const std::string &phpPath) : 
  _path(path),
  _pythonPath(pythonPath),
  _phpPath(phpPath)
{
  try {
    initCGI();
  }
  catch (Tools::Exception &e)
  {
    // Close clean here.
    throw;
  }
}

CGI::~CGI() {
  // Tools::closeAndResetFD(_pipeFDs[0]);
  // Tools::closeAndResetFD(_pipeFDs[1]);
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

const std::string &CGI::getPath() const { return _path; }
// const ConfigBase *CGI::getConfig() const { return _config; }
const std::string &CGI::getPythonPath() const { return _pythonPath; }
const std::string &CGI::getPhpPath() const { return _phpPath; }
// bool CGI::hasCGI() const { return _hasCGI; }

void CGI::setPath(const std::string &src) { _path = src; }
// void CGI::setConfig(ConfigBase *src) { _config = src; }
void CGI::setPythonPath(const std::string &src) { _pythonPath = src; }
void CGI::setPhpPath(const std::string &src) { _phpPath = src; }
// void CGI::setHasCGI(bool src) { _hasCGI = src; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/



void CGI::initCGI()
{
  
}

void CGI::executeScript(const HttpRequest &request)
{
  LOG(DEBUG, "SCRIPT");
  std::string output;
  std::string exec = "files" + request.getPurePath();

  int pipefd[2];
  if (pipe(pipefd) == -1)
  {
    throw Tools::Exception(500, "SCRIPT : Failed to create pipe");
  }
  pid_t pid = fork();
  if (pid == -1)
  {
    throw Tools::Exception(500, "SCRIPT : Failed to fork");
  }
  else if (pid == 0)
  {
    LOG(DEBUG, "CHILD");

    Tools::closeAndResetFD(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    Tools::closeAndResetFD(pipefd[1]);

    if (request.getPurePath() == "/cgi-bin/hello.py")
    {
      execl("/usr/bin/python3", "python3", exec.c_str(), NULL);
    }
    else if (request.getPurePath() == "/cgi-bin/info.php")
    {
      execl("/usr/bin/php", "php", exec.c_str(), NULL);
    }
    else if (request.getPurePath() == "/cgi-bin/getTime.py")
    {
      execl("/usr/bin/python3", "python3", exec.c_str(), NULL);
    }

    throw Tools::Exception(42, "CGI: child failed to execute");

  }
  else
  {
    LOG(DEBUG, "PAPA");
    Tools::closeAndResetFD(pipefd[1]);
    _pipeOut = pipefd[0];
    // LOG(DEBUG, PINK, Tools::intToString(pipefd[0]));
    // LOG(DEBUG, PINK, Tools::intToString(pipefd[1]));
    // LOG(DEBUG, PINK, Tools::intToString(_pipeOut));
  }
}
bool CGI::handleCGI()
{
  char buffer[4096];
  std::memset(buffer, '\0', 4096);
  ssize_t bytesRead;
  bytesRead = read(_pipeOut, buffer, sizeof(buffer));
  _buffer += buffer;
  if (_buffer.find("EOF")) {
    std::cout << _buffer << std::endl;
    // LOG(DEBUG, PINK, Tools::intToString(_pipeOut));
    Tools::closeAndResetFD(_pipeOut);
    // LOG(DEBUG, PINK, Tools::intToString(_pipeOut));
    return (true);
  }
  return (false);

  
	// le parent lit le pipe out a chaque fois et remplit le buffer
	// si il lit EOF a la fin
		// on remplit le buffer une derniere fois
		// on construit la reponse et la return
	

  // Event can be the read or write part of the pipe
  // It need to read from the pipe (the child is writing from the other side), and append this to the CGI _buffer
  // Then, once it has been determined that the CGI is finished, it will set a response and call handleResponse()
}
