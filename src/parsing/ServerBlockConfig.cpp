#include "parsing/ServerBlockConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
// ServerBlockConfig::ServerBlockConfig() {}

ServerBlockConfig::~ServerBlockConfig() {}

// ServerBlockConfig::ServerBlockConfig(const ServerBlockConfig &obj) { *this = obj; }

// Exception on parsing error
ServerBlockConfig::ServerBlockConfig(std::ifstream *infile) :
//	_duplicates(NONE),
	_infile(infile)
	{
	std::string line;

	while (std::getline(*_infile, line)) {
		// std::cout << line << std::endl;
		if (line[0] == '#' || line.empty())
			continue;
		else if (line[0] == '}')
			return;
		handleDirectiveName(line);
	}
	printData();
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

const std::string& ServerBlockConfig::getPort() const { return _port; }
const std::set<std::string>& ServerBlockConfig::getServerNames() const { return _serverNames; }
const std::map<std::string, LocationConfig>& ServerBlockConfig::getLocationConfigs() const { return _locationConfigs; }

void ServerBlockConfig::setPort(const std::string& src) { _port = src; }
void ServerBlockConfig::setServerNames(const std::set<std::string>& src) { _serverNames = src; }
void ServerBlockConfig::setLocationConfigs(const std::map<std::string, LocationConfig>& src) { _locationConfigs = src; }

/*
=================================================================
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

const std::map<std::string, ServerBlockConfig::DirectiveHandler> ServerBlockConfig::_serverHandlers
	= ServerBlockConfig::_initHandlers();

const std::map<std::string, ServerBlockConfig::DirectiveHandler> ServerBlockConfig::_initHandlers() {
	std::map<std::string, ServerBlockConfig::DirectiveHandler> temp;

	temp.insert(std::make_pair("listen", &ServerBlockConfig::parseListen));
	temp.insert(std::make_pair("server_name", &ServerBlockConfig::parseServerName));
	temp.insert(std::make_pair("location", &ServerBlockConfig::parseLocation));
	temp.insert(std::make_pair("root", &ServerBlockConfig::parseRoot));
	temp.insert(std::make_pair("index", &ServerBlockConfig::parseIndex));
	temp.insert(std::make_pair("autoindex", &ServerBlockConfig::parseAutoindex));
	temp.insert(std::make_pair("client_max_body_size", &ServerBlockConfig::parseClientMaxBodySize));
	temp.insert(std::make_pair("error_page", &ServerBlockConfig::parseErrorPage));
	temp.insert(std::make_pair("allow_methods", &ServerBlockConfig::parseAllowMethods));
	temp.insert(std::make_pair("return", &ServerBlockConfig::parseReturn));

	return temp;
}

/*
=================================================================
===== DIRECTIVE HANDLERS ========================================
=================================================================
*/

bool ServerBlockConfig::parseListen(std::vector<std::string>& tokens) {
	std::string port(tokens[1]);
	if (tokens.size() != 2 || !Tools::checkAndRemoveSemicolon(port))
		return false;

	if (port.size() > 5 && !Tools::isNumber(port))
		return false;

	int portStr = std::atoi(port.c_str());

	// unsigned short max is 65535
	if (portStr <= 0 || portStr > std::numeric_limits<unsigned short>::max())
		return false;

	_port = Tools::intToString(portStr);
	return true;
}

bool ServerBlockConfig::parseServerName(std::vector<std::string>& tokens) {	
	if (tokens.size() < 2)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens.back()))
		return false;

	tokens.erase(tokens.begin());
	_serverNames = std::set<std::string>(tokens.begin(), tokens.end());
	return true;
}

bool ServerBlockConfig::parseRoot(const std::vector<std::string>& tokens) {
	if (handleRoot(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseIndex(const std::vector<std::string>& tokens) {
	if (handleIndex(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseAutoindex(const std::vector<std::string>& tokens) {
	if (handleAutoindex(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseClientMaxBodySize(const std::vector<std::string>& tokens) {
	if (handleClientMaxBodySize(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseErrorPage(const std::vector<std::string>& tokens) {
	if (handleErrorPage(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseLocation(std::vector<std::string>& tokens) {
	if (tokens.size() < 2 || tokens.size() > 3)
		return false;

	std::string locationPath = tokens[1];
	if (_locationConfigs.find(locationPath) != _locationConfigs.end())
		throw Tools::Exception("Duplicate location path found");
	std::vector<std::string>::iterator it = tokens.begin();
	std::advance(it, 1);
	tokens.erase(it);
	LocationConfig lc(_infile, static_cast<const ConfigBase &>(*this));
	if (lc.parseLocationBlock(tokens)) {
		// _locationConfig = _locationConfig.getLocation();
		_locationConfigs.insert(std::make_pair(locationPath, lc));
		return true;
	}
	return false;
}

bool ServerBlockConfig::parseAllowMethods(const std::vector<std::string>& tokens) {
	if (handleAllowMethods(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseReturn(const std::vector<std::string>& tokens) {
	if (handleReturn(tokens, _infile))
		return true;
	return false;
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool ServerBlockConfig::handleStartingBrace(bool startingBraceIncluded) {
	if (!startingBraceIncluded) {
		std::string line;
		std::getline(*_infile, line);
		std::vector<std::string> tokens(Tools::splitString(line));
		if (tokens[0] != "{" || tokens.size() != 1)
			return false;
	}
	return true;
}

void ServerBlockConfig::handleDirectiveName(const std::string& line) {
	std::vector<std::string> tokens(Tools::splitString(line));
	if (tokens.size() < 2)
		throw Tools::Exception("Parsing error");
	
	std::map<std::string, ServerBlockConfig::DirectiveHandler>::const_iterator it;
	it = _serverHandlers.find(tokens[0]);
	if (it == _serverHandlers.end())
		throw Tools::Exception("Unknown directive name " + tokens[0]);
	
	DirectiveHandler handler = it->second;
	if (!(this->*handler)(tokens))
		throw Tools::Exception(tokens[0] + " directive not valid");
}

void ServerBlockConfig::printData() const {
	std::cout << "port: " << _port << std::endl;

	std::cout << "server names: ";
	std::set<std::string>::const_iterator it = _serverNames.begin();
	for (; it != _serverNames.end(); it++) {
		std::cout << *it << ", ";
	}
	std::cout << std::endl;

	ConfigBase::printData();
	std::cout << std::endl;

	std::map<std::string, LocationConfig>::const_iterator mit = _locationConfigs.begin();
	std::cout << "LocationCondig data" << std::endl;
	for (; mit != _locationConfigs.end(); mit++) {
		std::cout << "location path: " << mit->first << std::endl;
		mit->second.printData();
		std::cout << std::endl;
	}
	std::cout << std::endl;
	
}

void ServerBlockConfig::initWithDefaultData() {
	setPort("8080");							// port
	ConfigBase::initWithDefaultData();			// ConfigBase attributes
}
