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
		std::cout << line << std::endl;
		if (line[0] == '#' || line.empty())
			continue;
		else if (line[0] == '}')
			return;
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

bool ServerBlockConfig::parseListen(const std::vector<std::string>& tokens) {
	DirectiveHandlers dh(_infile);
	if (dh.handleListen(tokens)) {
		_port = dh.getListen();
		return true;
	}
	return false;
}

bool ServerBlockConfig::parseServerName(const std::vector<std::string>& tokens) {	
	DirectiveHandlers dh(_infile);
	if (dh.handleServerName(tokens)) {
		_serverNames = dh.getServerName();
		return true;
	}
	return false;
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

bool ServerBlockConfig::parseLocation(const std::vector<std::string>& tokens) {
	DirectiveHandlers dh(_infile);
	if (dh.handleLocation(tokens)) {
		_locationConfigs.insert(dh.getLocation());
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

