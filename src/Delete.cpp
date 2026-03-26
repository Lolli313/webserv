#include "Delete.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Delete::Delete() {}

Delete::~Delete() {}

Delete::Delete(const Delete &obj) { *this = obj; }

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
Delete &Delete::operator=(const Delete &obj)
{
	if (this != &obj)
	{
	}
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

const std::string Delete::executeDelete(const HttpRequest &request, const ConfigBase *config) {
	std::string fullPath = config->getRoot() + request.getPurePath();
	std::clog << fullPath << std::endl;
	std::ifstream infile(fullPath.c_str());
	if (!infile.is_open()) {
		throw Tools::Exception(404, "File doesn't exist");
	}
	infile.close();
	if (std::remove(fullPath.c_str()) != 0) {
		throw Tools::Exception(500, "Error deleting file");
	}

	return HttpResponse::quickHttpResponse(HttpTools::getReturnPair(204));
}
