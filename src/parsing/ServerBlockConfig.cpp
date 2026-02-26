#include "parsing/ServerBlockConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
// ServerBlockConfig::ServerBlockConfig() {}

ServerBlockConfig::~ServerBlockConfig() {}

// ServerBlockConfig::ServerBlockConfig(const ServerBlockConfig &obj) { *this = obj; }

ServerBlockConfig::ServerBlockConfig(std::ifstream& infile, bool startingBraceIncluded) :
	_infile(infile) 
	{
	std::cout << "Hello from inside the ServerBlockConfig" << std::endl;
	std::string line;
	if (!handleStartingBrace(startingBraceIncluded)) {
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

bool ServerBlockConfig::handleStartingBrace(bool startingBraceIncluded) {
	if (!startingBraceIncluded) {
		std::string line;
		std::getline(_infile, line);
		std::vector<std::string> tokens(Tools::splitString(line));
		if (tokens[0] != "{" || tokens.size() != 1)
			return false;
	}
	return true;
}
