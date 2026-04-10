#pragma once

#include "HttpTools.hpp"

#include <string>

// BASE CLASS, CANNOT BE INSTANTIATED
class HttpMethod
{
private:
	HttpMethod();

	std::string _type;

public:
	HttpMethod(const HttpMethod &obj);
	HttpMethod &operator=(const HttpMethod &obj);
	~HttpMethod();
};
