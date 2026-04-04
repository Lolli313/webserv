#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "ConfigBase.hpp"
#include "HttpTools.hpp"
#include "Tools.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

class CGI
{
private:
	const HttpRequest &_request;
	const ConfigBase *_config;
	std::string _cgiBinPath;
	// ConfigBase* _config;
	std::string _executablePath;
	std::string _buffer;
	std::string _scriptName;
	int _pid;
	int _pipeFDs[2];
	int _postPipesFDs[2];
	int _responseStatus;

	std::size_t _postPos;

public:
	// CGI() {};
	CGI(const HttpRequest& request, const ConfigBase *config);
	CGI(const CGI &obj);
	CGI &operator=(const CGI &obj);
	~CGI();

	// Getters
	// CGI POST INPUT PIPE FD
	int getPostPipeIn() const { return _postPipesFDs[1]; }
	int getPostPipeOut() const { return _postPipesFDs[0]; }

	// CGI OUTPUT PIPE FD
	int getPipeOut() const { return _pipeFDs[0]; }
	const std::string& getPath() const;
	// const ConfigBase *getConfig() const;
	const std::string& getBuffer() const {return _buffer;}
	// bool hasCGI() const;

	// Setters
	void setPath(const std::string& src);
	// void setConfig(ConfigBase* src);
	// void setHasCGI(bool src);

	// void cleanClose(int *fd);
	void executeCGI();
	bool readCgiOutput();
	void setCGI();
	void pipeAndFork();
	void setPostPipe();
	bool handlePostCGI();
	std::string checkAndExtractScript();
	void setChildPipe();
	void setPostDup();
	void buildParam(std::string& fullScriptPath, char* param[3]);
	void buildEnv(char**& env, std::vector<std::string>& envBuildVector);
	const std::string getResponse();

	std::vector<std::pair<std::string, std::string> > parseOutput();
};
