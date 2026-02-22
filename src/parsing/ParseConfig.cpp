
#include "parsing/ParseConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
ParseConfig::ParseConfig() {};

ParseConfig::~ParseConfig() {};

ParseConfig::ParseConfig(const ParseConfig &obj) { *this = obj; };

ParseConfig::ParseConfig(const std::string& filePath) : _fileName(filePath) {
	std::ifstream infile(_fileName.c_str());
	if (!infile.is_open())
		throw Tools::Exception("Error opening config file");
	
}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
ParseConfig &ParseConfig::operator=(const ParseConfig &obj)
{
	if (this != &obj)
	{
	}
	return (*this);
};

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

