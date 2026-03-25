#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ConfigBase.hpp"

class Delete
{
private:
public:
	Delete();
	Delete(const Delete &obj);
	Delete &operator=(const Delete &obj);
	~Delete();

	static const std::string executeDelete(const HttpRequest &request, const ConfigBase *config);
};
