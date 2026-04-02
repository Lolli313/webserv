#pragma once

#include "ConfigBase.hpp"
#include "HttpRequest.hpp"
#include "Tools.hpp"
#include <string>

class CGI
{
private:
	std::string _path;
	// ConfigBase* _config;
	std::string _pythonPath;
	std::string _phpPath;
	std::string _buffer;
	int _pipeOut;

public:
	CGI() {};
	CGI(const std::string &path, const std::string &pythonPath, const std::string &phpPath);
	CGI(const CGI &obj);
	CGI &operator=(const CGI &obj);
	// ~CGI();

	// Getters
	int getPipeOut() const {return _pipeOut;}
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
	void initCGI();
	void executeScript(const HttpRequest &request);
	bool handleCGI();
};
