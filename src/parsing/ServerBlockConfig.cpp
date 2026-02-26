#include "parsing/ServerBlockConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
// ServerBlockConfig::ServerBlockConfig() {}

ServerBlockConfig::~ServerBlockConfig() {}

ServerBlockConfig::ServerBlockConfig(const ServerBlockConfig &obj) { *this = obj; }

ServerBlockConfig::ServerBlockConfig(std::ifstream& infile, bool startingBraceIncluded) {
	std::cout << "Hello from inside the ServerBlockConfig" << std::endl;
	// (void)infile;
	// (void)startingBraceIncluded;
	std::string line;
	if (!startingBraceIncluded) {
		std::getline(infile, line);
		std::vector<std::string> tokens(Tools::splitString(line));
		if (tokens[0] != "{" || tokens.size() != 1)
			throw Tools::Exception("Parsing error");
	}

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
