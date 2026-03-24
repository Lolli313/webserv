#pragma once

#include "string"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "fstream"

class Get
{
private:
	std::ifstream _file;
	std::string &_host;

	const HttpRequest &_request;
public:
	Get(const HttpRequest &request);
	Get(const Get &obj);
	Get &operator=(const Get &obj);
	~Get();
	
	void checkRequest();	
	void checkAndSetFile(const std::string &path);

	const std::string executeGet(const HttpRequest &request);
};
