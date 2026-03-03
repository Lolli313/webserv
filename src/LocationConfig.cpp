#include "LocationConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

LocationConfig::LocationConfig() {}

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig &obj) : ConfigBase(obj) { *this = obj; }

LocationConfig::LocationConfig(std::ifstream *infile) : ConfigBase(*this), _infile(infile)
{
}

LocationConfig::LocationConfig(std::ifstream *infile, const ConfigBase &config) : ConfigBase(config), _infile(infile)
{
}

LocationConfig::LocationConfig(const ConfigBase &obj) : ConfigBase(obj)
{
}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

// LocationConfig &LocationConfig::operator=(const LocationConfig &obj)
// {
// 	if (this != &obj)
// 		ConfigBase::operator=(obj);
// 	return (*this);
// }

LocationConfig &LocationConfig::operator=(const ConfigBase &obj)
{
	if (this != &obj)
		ConfigBase::operator=(obj);
	return (*this);
}

// LocationConfig &LocationConfig::operator=(const Server &obj)
// {
// 	ConfigBase::operator=(static_cast<const ConfigBase &>(obj));
// 	return (*this);
// }


// LocationConfig &LocationConfig::operator=(const ServerBlockConfig &obj)
// {
// 	ConfigBase::operator=(static_cast<const ConfigBase &>(obj));
// 	return (*this);
// }


/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const LocationConfig &LocationConfig::getLocation() const
{
	return *this;
}

/*
=================================================================
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

const std::map<std::string, LocationConfig::DirectiveHandler> LocationConfig::_locationHandlers = LocationConfig::_initHandlers();

const std::map<std::string, LocationConfig::DirectiveHandler> LocationConfig::_initHandlers()
{
	std::map<std::string, LocationConfig::DirectiveHandler> temp;

	temp.insert(std::make_pair("root", &ConfigBase::handleRoot));
	temp.insert(std::make_pair("index", &ConfigBase::handleIndex));
	temp.insert(std::make_pair("autoindex", &ConfigBase::handleAutoindex));
	temp.insert(std::make_pair("client_max_body_size", &ConfigBase::handleClientMaxBodySize));
	temp.insert(std::make_pair("error_page", &ConfigBase::handleErrorPage));
	temp.insert(std::make_pair("allow_methods", &ConfigBase::handleAllowMethods));
	temp.insert(std::make_pair("return", &ConfigBase::handleReturn));

	return temp;
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool LocationConfig::parseLocationBlock(const std::vector<std::string> &tokens)
{
	// if (tokens.size() < 2 || tokens.size() > 3)
	// 	return false;

	// std::string path(tokens[1]);
	// std::vector<std::string> tempTokens(tokens);
	// std::vector<std::string>::iterator it = tempTokens.begin();
	// std::advance(it, 1);
	// tempTokens.erase(it);
	if (!Tools::isValidBraceFormat("location", tokens, _infile))
		return false;

	std::string line;
	while (std::getline(*_infile, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		// std::cout << line << std::endl;
		std::vector<std::string> tokens = Tools::splitString(line);
		if (tokens[0] == "}")
			return true;

		std::map<std::string, LocationConfig::DirectiveHandler>::const_iterator it;
		it = _locationHandlers.find(tokens[0]);
		if (it == _locationHandlers.end())
			return false;

		DirectiveHandler handler = it->second;
		if (!(this->*handler)(tokens, _infile))
			return false;
	}
	return false;
}

void LocationConfig::printData() const {
	ConfigBase::printData();
}
