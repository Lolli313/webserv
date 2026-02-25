#include "parsing/ServerBlockConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
ServerBlockConfig::ServerBlockConfig() {}

ServerBlockConfig::~ServerBlockConfig() {}

ServerBlockConfig::ServerBlockConfig(const ServerBlockConfig &obj) { *this = obj; }

ServerBlockConfig::ServerBlockConfig(std::ifstream& infile, bool startingBraceIncluded) {
	std::cout << "Hello from inside the ServerBlockConfig" << std::endl;
	(void)infile;
	(void)startingBraceIncluded;
}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
ServerBlockConfig &ServerBlockConfig::operator=(const ServerBlockConfig &obj)
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
