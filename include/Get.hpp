#pragma once

#include "string"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "fstream"
#include "Tools.hpp"
#include "fcntl.h"
#include "ConfigBase.hpp"
#include "Polling.hpp"

class Get
{
private:
	const HttpRequest &_request;
	const ConfigBase *_config;
	std::string _host;
	std::string _file;

public:
	Get(const HttpRequest &request, const ConfigBase *config);
	// Get(const Get &obj);
	// Get &operator=(const Get &obj);
	~Get();

	void checkRequest();
	void checkAndSetFile(const std::string &path);

	static const std::string executeGet(const HttpRequest &request, const ConfigBase *config);
};
