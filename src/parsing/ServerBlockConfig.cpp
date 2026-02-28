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
	_duplicates(NONE),
	_infile(infile)
	{
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
===== DIRECTIVE HANDLERS ========================================
=================================================================
*/

const std::map<std::string, ServerBlockConfig::DirectiveHandler> ServerBlockConfig::_handlers
	= ServerBlockConfig::_initHandlers();

const std::map<std::string, ServerBlockConfig::DirectiveHandler> ServerBlockConfig::_initHandlers() {
	std::map<std::string, ServerBlockConfig::DirectiveHandler> temp;

	temp.insert(std::make_pair("listen", &ServerBlockConfig::handleListen));
	temp.insert(std::make_pair("server_name", &ServerBlockConfig::handleServerName));
	temp.insert(std::make_pair("root", &ServerBlockConfig::handleRoot));
	temp.insert(std::make_pair("index", &ServerBlockConfig::handleIndex));
	temp.insert(std::make_pair("autoindex", &ServerBlockConfig::handleAutoindex));
	temp.insert(std::make_pair("client_max_body_size", &ServerBlockConfig::handleClientMaxBodySize));
	temp.insert(std::make_pair("error_page", &ServerBlockConfig::handleErrorPage));
	temp.insert(std::make_pair("location", &ServerBlockConfig::handleLocation));
	temp.insert(std::make_pair("allow_methods", &ServerBlockConfig::handleAllowMethods));
	temp.insert(std::make_pair("return", &ServerBlockConfig::handleReturn));

	return temp;
}

bool ServerBlockConfig::handleListen(const std::vector<std::string>& tokens) {
	std::string port(tokens[1]);
	if (tokens.size() != 2 || !Tools::checkAndRemoveSemicolon(port))
		return false;
	if (port.size() > 5 && !Tools::isNumber(port))
		return false;
	int portStr = std::atoi(port.c_str());
	if (portStr > std::numeric_limits<unsigned short>::max())
		return false;

	_port = portStr;
	return true;
}

bool ServerBlockConfig::handleServerName(const std::vector<std::string>& tokens) {	
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleRoot(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleIndex(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleAutoindex(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleClientMaxBodySize(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleErrorPage(const std::vector<std::string>& tokens) {
	if (!Tools::isValidBraceFormat("error_page", tokens, _infile))
		return false;
	std::map<int, std::string> temp;
	std::string line;
	while (std::getline(_infile, line)) {
		if (line.empty() || line[0] == '#')
			continue;

		std::cout << "line is: " << line << std::endl;
		std::vector<std::string> tokens = Tools::splitString(line);

		if (tokens[0] == "}") {
			setErrorPages(temp);
			return true;
		}

		if (tokens.size() != 2 || tokens[0].size() != 3 || !Tools::isNumber(tokens[0]))
			return false;

		int httpCode = std::atoi(tokens[0].c_str());
		if (!HttpTools::isValidHttpCode(httpCode))
			return false;

		std::string path(tokens[1]);
		if (!Tools::checkAndRemoveSemicolon(path))
			return false;
		temp.insert(std::make_pair(httpCode, path));
	}
	return false;
}

bool ServerBlockConfig::handleLocation(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleAllowMethods(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleReturn(const std::vector<std::string>& tokens) {
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
	it = _handlers.find(tokens[0]);
	if (it == _handlers.end()) {
		throw Tools::Exception("Unknown directive name " + tokens[0]);
	}
	
	DirectiveHandler handler = it->second;
	if (!(this->*handler)(tokens))
		throw Tools::Exception(tokens[0] + " directive not valid");
}

