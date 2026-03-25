#include "CGI.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
CGI::CGI() {
	initCGI();
}

CGI::~CGI() {}

CGI::CGI(const CGI &obj) { *this = obj; }

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
CGI &CGI::operator=(const CGI &obj)
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

const std::string& CGI::getPath() const { return _path; }
const ConfigBase *CGI::getConfig() const { return _config; }
const std::string& CGI::getPythonPath() const { return _pythonPath; }
const std::string& CGI::getPhpPath() const { return _phpPath; }
bool CGI::hasCGI() const { return _hasCGI; }


void CGI::setPath(const std::string& src) { _path = src; }
void CGI::setConfig(ConfigBase* src) { _config = src; }
void CGI::setPythonPath(const std::string& src) { _pythonPath = src; }
void CGI::setPhpPath(const std::string& src) { _phpPath = src; }
void CGI::setHasCGI(bool src) { _hasCGI = src; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

void CGI::initCGI() {
	setPath("");
	setConfig(NULL);
	setPythonPath("");
	setPhpPath("");
	setHasCGI(false);
}
