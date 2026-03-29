#include "parsing/ServerBlockConfig.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ServerBlockConfig::~ServerBlockConfig() {}

/**
 * @throws handleDirectiveName() throws Tools::Exception on parsing error
 */
ServerBlockConfig::ServerBlockConfig(std::ifstream *infile) : _infile(infile) {
	std::string line;
	while (std::getline(*_infile, line)) {
		if (Tools::lineIsEmptyOrComment(line))
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

ServerBlockConfig &ServerBlockConfig::operator=(const ServerBlockConfig &obj) {
	if (this != &obj) {
		_port = obj._port;
		_serverNames = obj._serverNames;
		_locationConfigs = obj._locationConfigs;
		_infile = obj._infile;
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
	temp.insert(std::make_pair("cgi", &ServerBlockConfig::parseCgi));

	return temp;
}

/*
=================================================================
===== DIRECTIVE HANDLERS ========================================
=================================================================
*/

bool ServerBlockConfig::parseListen(std::vector<std::string>& tokens) {
	std::string& port(tokens[1]);
	if (tokens.size() != 2 || !Tools::checkAndRemoveSemicolon(port))
		return false;

	if (!Tools::isValidPort(port))
		return false;
	
	_port = port;
	return true;
}

bool ServerBlockConfig::parseServerName(std::vector<std::string>& tokens) {	
	if (tokens.size() < 2)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens.back()))
		return false;

	for (std::vector<std::string>::iterator cit = tokens.begin() + 1; cit != tokens.end(); cit++)
		Tools::transformStringToLowecase(*cit);

	_serverNames = std::set<std::string>(tokens.begin() + 1, tokens.end());
	return true;
}

bool ServerBlockConfig::parseRoot(std::vector<std::string>& tokens) {
	if (handleRoot(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseIndex(std::vector<std::string>& tokens) {
	if (handleIndex(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseAutoindex(std::vector<std::string>& tokens) {
	if (handleAutoindex(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseClientMaxBodySize(std::vector<std::string>& tokens) {
	if (handleClientMaxBodySize(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseErrorPage(std::vector<std::string>& tokens) {
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

	if (!Tools::stringStartsWithCharacter(locationPath, '/'))
		return false;

	std::vector<std::string>::iterator it = tokens.begin();
	std::advance(it, 1);
	tokens.erase(it);
	LocationConfig lc(_infile, static_cast<const ConfigBase &>(*this));
	if (lc.parseLocationBlock(tokens)) {
		_locationConfigs.insert(std::make_pair(locationPath, lc));
		return true;
	}
	return false;
}

bool ServerBlockConfig::parseAllowMethods(std::vector<std::string>& tokens) {
	if (handleAllowMethods(tokens, _infile))
		return true;
	return false;
}

bool ServerBlockConfig::parseReturn(std::vector<std::string>& tokens) {
	if (handleReturn(tokens, _infile))
		return true;
	return false;
}

bool checkCgiDirectiveValidity(std::vector<std::string>& tokens) {
	if (tokens.size() != 2)
		return false;
	
	std::string& path = tokens[1];
	if (!Tools::checkAndRemoveSemicolon(path) || path.empty() || !Tools::stringStartsWithCharacter(path, '/'))
		return false;

	return true;
}

bool ServerBlockConfig::parseCgi(std::vector<std::string>& tokens) {
	if (tokens.size() < 2 || tokens.size() > 3)
		return false;

	if (!Tools::isValidBraceFormat("cgi", tokens, _infile))
		return false;

	std::string line;
	while (std::getline(*_infile, line)) {
		if (Tools::lineIsEmptyOrComment(line))
			continue;

		tokens = Tools::splitString(line);
		if (tokens[0] == "}")
			return true;
		
		if (!checkCgiDirectiveValidity(tokens))
			return false;

		const std::string& key = tokens[0];
		const std::string& value = tokens[1];

		if (key == "path")
			_cgi.setPath(value);
		else if (key == "python")
			_cgi.setPythonPath(value);
		else if (key == "php")
			_cgi.setPhpPath(value);
		else
			return false;
	}
	_cgi.setHasCGI(true);
	return true;
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

/**
 * @brief valid server keyword formats:
 * 
 * ```
 * server { 
 * and
 * server
 * {
 * ```
 * 
 * @param startingBraceIncluded if false, check if starting brace is on the next line
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

/**
 * @brief Redirect to the appropriate parsing function
 * @throws Tools::Exception if directive name or format is not valid
 */
void ServerBlockConfig::handleDirectiveName(const std::string& line) {
	std::vector<std::string> tokens(Tools::splitString(line));
	if (tokens.size() < 2) {
		throw Tools::Exception("Parsing error");
	}
	std::map<std::string, ServerBlockConfig::DirectiveHandler>::const_iterator it;
	it = _serverHandlers.find(tokens[0]);
	if (it == _serverHandlers.end()) {
		throw Tools::Exception("Unknown directive name " + tokens[0]);
	}
	DirectiveHandler handler = it->second;
	if (!(this->*handler)(tokens)) {
		throw Tools::Exception(tokens[0] + " directive not valid");
	}
}

void ServerBlockConfig::printData() const {
    // 1. Port
    LOG(DEBUG, GREEN, "Port", _port);

    // 2. Server Names (Joined List)
    std::string names;
    for (std::set<std::string>::const_iterator it = _serverNames.begin(); it != _serverNames.end(); ++it) {
        if (!names.empty()) names += ", ";
        names += *it;
    }
    LOG(DEBUG, GREEN, "Server Names", (names.empty() ? "none" : names));

    // 3. Parent Class Data
    // This will print Root, Indexes, Autoindex, etc., using the logic we just wrote.
    ConfigBase::printData();

    // 4. Location Configurations
    if (!_locationConfigs.empty()) {
        LOG(DEBUG, GREEN, "--- LocationConfig Data ---", "");
        
        std::map<std::string, LocationConfig>::const_iterator mit = _locationConfigs.begin();
        for (; mit != _locationConfigs.end(); ++mit) {
            // Highlight the specific path for this location block
            LOG(DEBUG, GREEN, "Location Path", mit->first);
            
            // Recurse into the LocationConfig's own printData
            mit->second.printData();
            
            // Visual separator between different location blocks
            LOG(DEBUG, GREEN, "----------------------------");
        }
    } else {
        LOG(DEBUG, GREEN, "LocationConfigs", "none");
    }
}

void ServerBlockConfig::initWithDefaultData() {
	setPort("8080");							// port
	ConfigBase::initWithDefaultData();			// ConfigBase attributes
}
