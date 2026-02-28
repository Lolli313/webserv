#include "LocationConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
LocationConfig::LocationConfig() {}

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig &obj) { *this = obj; }

LocationConfig::LocationConfig(std::ifstream& infile) {
	parseLocationBlock(infile);
}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
LocationConfig &LocationConfig::operator=(const LocationConfig &obj)
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

bool LocationConfig::parseLocationBlock(std::ifstream& infile) {
	std::string line;
	while (std::getline(infile, line)) {
		if (line.empty() || line[0] == '#')
			continue;

		std::cout << "line is: " << line << std::endl;
		std::vector<std::string> tokens = Tools::splitString(line);

		if (tokens[0] == "}") {
//			setErrorPages(temp);
			return true;
		}
	}
	return true;
}
