#pragma once

#include "ConfigBase.hpp"
#include "HttpRequest.hpp"
#include "Tools.hpp"
#include <string>

class CGI
{
private:
	std::string _cgiBinPath;
	// ConfigBase* _config;
	std::string _pythonPath;
	std::string _phpPath;
	std::string _buffer;
	std::string _responseBuffer;
	int _pid;
	int _pipeFDs[2];
	int _postPipesFDs[2];

public:
	CGI() {};
	CGI(const HttpRequest& request, const ConfigBase *config);
	CGI(const CGI &obj);
	CGI &operator=(const CGI &obj);
	~CGI();

	// Getters
	// CGI POST INPUT PIPE FD
	int getPostPipeIn() const { return _postPipesFDs[1]; }

	// CGI OUTPUT PIPE FD
	int getPipeOut() const { return _pipeFDs[0]; }
	const std::string& getPath() const;
	// const ConfigBase *getConfig() const;
	const std::string& getPythonPath() const;
	const std::string& getPhpPath() const;
	const std::string& getBuffer() const {return _buffer;}
	// bool hasCGI() const;

	// Setters
	void setPath(const std::string& src);
	// void setConfig(ConfigBase* src);
	void setPythonPath(const std::string& src);
	void setPhpPath(const std::string& src);
	// void setHasCGI(bool src);

	// void cleanClose(int *fd);
	void executeCGI(const HttpRequest &request, const ConfigBase* config);
	bool handleCGI();
	void setCGI(const HttpRequest &request, const ConfigBase* config);
	void pipeAndFork();
	void handlePostCGI();
};
