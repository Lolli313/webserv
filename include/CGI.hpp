#pragma once

#include "ConfigBase.hpp"
#include "HttpRequest.hpp"

#include <string>

class CGI
{
private:
	std::string _path;
	ConfigBase* _config;
	std::string _pythonPath;
	std::string _phpPath;
	bool _hasCGI;

public:
	CGI();
	CGI(const CGI &obj);
	CGI &operator=(const CGI &obj);
	~CGI();

	// Getters
	const std::string& getPath() const;
	const ConfigBase *getConfig() const;
	const std::string& getPythonPath() const;
	const std::string& getPhpPath() const;
	bool hasCGI() const;

	// Setters
	void setPath(const std::string& src);
	void setConfig(ConfigBase* src);
	void setPythonPath(const std::string& src);
	void setPhpPath(const std::string& src);
	void setHasCGI(bool src);

	void initCGI();
	static const std::string executeScript(const HttpRequest &request);

};
