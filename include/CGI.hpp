#pragma once

#include "ConfigBase.hpp"
#include "HttpRequest.hpp"
#include "Tools.hpp"
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
	std::string _responseBuffer;
	int _pid;
	int _pipeFDs[2];
	int _postPipesFDs[2];

	std::size_t _postPos;

public:
	CGI() {};
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
	const std::string checkAndExtractScript();
	void setChildPipe();
	void setPostDup();
	char **buildParam(std::string& fullScriptPath);
	char **buildEnv();
};
