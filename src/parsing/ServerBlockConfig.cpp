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
ServerBlockConfig::ServerBlockConfig(std::ifstream& infile) :
//	_duplicates(NONE),
	_infile(infile)
	{
//	_initDH(_infile);
	std::cout << "Hello from inside the ServerBlockConfig" << std::endl;
	std::string line;

	while (std::getline(_infile, line)) {
		std::cout << line << std::endl;
		if (line[0] == '#' || line.empty())
			continue;
		handleDirectiveName(line);
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
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

const std::map<std::string, ServerBlockConfig::DirectiveHandler> ServerBlockConfig::_serverHandlers
	= ServerBlockConfig::_initHandlers(true);

const std::map<std::string, ServerBlockConfig::DirectiveHandler> ServerBlockConfig::_locationHandlers
	= ServerBlockConfig::_initHandlers(false);

const std::map<std::string, ServerBlockConfig::DirectiveHandler> ServerBlockConfig::_initHandlers(bool forServer) {
	std::map<std::string, ServerBlockConfig::DirectiveHandler> temp;

	if (forServer) {
		temp.insert(std::make_pair("listen", &ServerBlockConfig::parseListen));
		temp.insert(std::make_pair("server_name", &ServerBlockConfig::parseServerName));
		temp.insert(std::make_pair("location", &ServerBlockConfig::parseLocation));
	}
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

bool ServerBlockConfig::parseListen(const std::vector<std::string>& tokens) {
	// std::string port(tokens[1]);
	// if (tokens.size() != 2 || !Tools::checkAndRemoveSemicolon(port))
	// 	return false;
	// if (port.size() > 5 && !Tools::isNumber(port))
	// 	return false;
	// int portStr = std::atoi(port.c_str());
	// if (portStr > std::numeric_limits<unsigned short>::max())
	// 	return false;

	// _port = portStr;
	// return true;
	DirectiveHandlers dh(_infile);
	if (dh.handleListen(tokens)) {
		_port = dh.getListen();
		return true;
	}
	return false;

}

bool ServerBlockConfig::parseServerName(const std::vector<std::string>& tokens) {	
	(void)tokens;
	return true;
}

bool ServerBlockConfig::parseRoot(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::parseIndex(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::parseAutoindex(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::parseClientMaxBodySize(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::parseErrorPage(const std::vector<std::string>& tokens) {
	DirectiveHandlers dh(_infile);
	if (dh.handleErrorPage(tokens)) {
		setErrorPages(dh.getErrorPages());
		return true;
	}
	return false;
	
}

bool ServerBlockConfig::parseLocation(const std::vector<std::string>& tokens) {
	(void)tokens;
	DirectiveHandlers dh(_infile);
	if (dh.handleLocation(tokens)) {
		_locationConfigs = dh.getLocation();
		return true;
	}
	return true;
	// if (tokens.size() < 2 || tokens.size() > 3)
	// 	return false;

	// std::string path(tokens[1]);
	// std::vector<std::string> tempTokens(tokens);
	// std::vector<std::string>::iterator it = tempTokens.begin();
	// std::advance(it, 1);
	// tempTokens.erase(it);
	// if (!Tools::isValidBraceFormat("location", tempTokens, _infile))
	// 	return false;

	// std::string line;
	// while (std::getline(_infile, line)) {
	// 	if (line.empty() || line[0] == '#')
	// 		continue;

	// 	std::cout << "line is: " << line << std::endl;
	// 	std::vector<std::string> tokens = Tools::splitString(line);
	// 	if (tokens[0] == "}")
	// 		return true;

		
	// 	std::map<std::string, ServerBlockConfig::DirectiveHandler>::const_iterator it;
	// 	it = _locationHandlers.find()
	// }
	// return true;
}

bool ServerBlockConfig::parseAllowMethods(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::parseReturn(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

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

