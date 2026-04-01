
#include "Script.hpp"

#include <sys/wait.h>
#include <ctime>
#include <iomanip>
#include <sstream>

const std::string Script::executeScript(const HttpRequest &request) {
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
		
        execl("/usr/bin/python3", "python3", exec.c_str(), NULL);
		exit(1);

    } else { 
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
	return output;
}