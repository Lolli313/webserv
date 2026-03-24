#pragma once

#include "string"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class MethodGET
{
private:
	

public:
	MethodGET();
	MethodGET(const MethodGET &obj);
	MethodGET &operator=(const MethodGET &obj);
	~MethodGET();

	const std::string executeGet(const HttpRequest &request);
};
