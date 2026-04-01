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
CGI::CGI() {
	initCGI();
}

CGI::~CGI() {}

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

const std::string& CGI::getPath() const { return _path; }
const ConfigBase *CGI::getConfig() const { return _config; }
const std::string& CGI::getPythonPath() const { return _pythonPath; }
const std::string& CGI::getPhpPath() const { return _phpPath; }
bool CGI::hasCGI() const { return _hasCGI; }


void CGI::setPath(const std::string& src) { _path = src; }
void CGI::setConfig(ConfigBase* src) { _config = src; }
void CGI::setPythonPath(const std::string& src) { _pythonPath = src; }
void CGI::setPhpPath(const std::string& src) { _phpPath = src; }
void CGI::setHasCGI(bool src) { _hasCGI = src; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

void CGI::initCGI() {
	setPath("");
	setConfig(NULL);
	setPythonPath("");
	setPhpPath("");
	setHasCGI(false);
}

const std::string CGI::executeScript(const HttpRequest &request) {
	LOG(DEBUG, "SCRIPT");
	std::string output;
	std::string exec = "files" + request.getPurePath();

    int pipefd[2];
    if (pipe(pipefd) == -1) {
		throw Tools::Exception(500, "SCRIPT : Failed to create pipe");
    }
    pid_t pid = fork();

    if (pid == -1) {
		throw Tools::Exception(500, "SCRIPT : Failed to fork");

    } else if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        LOG(DEBUG, "CHILD");
        if (request.getPurePath() == "/cgi-bin/hello.py") {
          execl("/usr/bin/python3", "python3", exec.c_str(), NULL);
        } else if (request.getPurePath() == "/cgi-bin/info.php") {
          execl("/usr/bin/php", "php", exec.c_str(), NULL);
        }
        else if (request.getPurePath() == "/cgi-bin/getTime.py")
          execl("/usr/bin/python3", "python3", exec.c_str(), NULL);
		    exit(1);

    } else {
        LOG(DEBUG, "PAPA");
        close(pipefd[1]);

		char buffer[4096];
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			output.append(buffer, bytesRead);
		}

        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);
        if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
			throw Tools::Exception(500, "SCRIPT : Execution failed");
        }
    }
    LOG(DEBUG, "FINISH");
	return output;
}
