#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Delete
{
private:
public:
	Delete();
	Delete(const Delete &obj);
	Delete &operator=(const Delete &obj);
	~Delete();

	// static const std::string executeDelete(const HttpRequest &request);
};
