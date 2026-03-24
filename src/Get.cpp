#include "Get.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Get::Get(const HttpRequest &request) : _request(request) {}

Get::~Get() {}

Get::Get(const Get &obj) { *this = obj; }

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
Get &Get::operator=(const Get &obj)
{
	(void)obj;
	return (*this);
}

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

void Get::checkRequest()
{
	std::map<std::string, std::string>::const_iterator it = _request.getHeader().find("Host")
	_host = _request.getHeader().find("Host")->second;
}

void Get::checkAndSetFile(const std::string &path)
{
	_file.open(path);
	if (!_file.is_open())
	{
		throw 
	}
}