
#include "parsing/ParseConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
ParseConfig::ParseConfig() {};

ParseConfig::~ParseConfig() {};

ParseConfig::ParseConfig(const ParseConfig &obj) { *this = obj; };

/**
 * @brief Creates a "God parsing class object" that contains all of the server blocks
 * mentioned in the config file
 * @param filePath Path to the file to be parsed
 * @throws Tools::Exception if file cannot be opened or if a server block is not found 
 */
ParseConfig::ParseConfig(const std::string& filePath) : _fileName(filePath) {
	std::ifstream infile(_fileName.c_str());
	if (!infile.is_open())
		throw Tools::Exception("Error opening config file");
	std::string line;
	while (std::getline(infile, line)) {
		if (line.empty() || line[0] == '#')
			continue;
		else if (!checkServerKeyword(line, &infile))
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

const std::vector<ServerBlockConfig>& ParseConfig::getServerConfig() const {
	return _serverBlockConfigVector;
}


/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

/**
 * @brief valid server keyword formats
 * 
 * ```
 * server { 
 * and
 * server
 * {
 * ```
 */
bool ParseConfig::checkServerKeyword(const std::string& line, std::ifstream *infile) {
	if (line.compare(0, 6, "server") != 0)
		return false;
	std::vector<std::string> tokens(Tools::splitString(line));

	if (!Tools::isValidBraceFormat("server", tokens, infile))
		return false;
	_serverBlockConfigVector.push_back(ServerBlockConfig(infile));
	return true;
}

void ParseConfig::printData() const {
	std::vector<ServerBlockConfig>::const_iterator it = _serverBlockConfigVector.begin();
	for (; it != _serverBlockConfigVector.end(); it++) {
		it->printData();
		std::cout << std::endl;
	}
}
