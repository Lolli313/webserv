
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
	std::string line;
	while (std::getline(infile, line)) {
		std::cout << line << std::endl;
		if (line[0] == '#' || line.empty())
			continue;
		else if (checkServerKeyword(line, infile))
			throw Tools::Exception("Parsing error");
	}
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

/**
 * valid server keyword formats are:
 * "server {" and
 * "server
 * {"
 */
bool ParseConfig::checkServerKeyword(const std::string& line, std::ifstream& infile) {
	if (line.compare(0, 6, "server") != 0)
		return false;
	std::vector<std::string> tokens(Tools::splitString(line));

	bool validFormat =
		(tokens.size() == 1 && tokens[0] == "server") ||
		(tokens.size() == 2 && tokens[1] == "{");
	if (!validFormat)
		return false;
	_serverBlockConfigVector.push_back(ServerBlockConfig(infile, tokens.size() == 2));
	return true;
}


