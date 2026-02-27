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
ServerBlockConfig::ServerBlockConfig(std::ifstream& infile, bool startingBraceIncluded) :
	_duplicates(NONE),
	_infile(infile)
	{
	std::cout << "Hello from inside the ServerBlockConfig" << std::endl;
	std::string line;

	if (!handleStartingBrace(startingBraceIncluded)) {
		throw Tools::Exception("Parsing error");
	}
	while (std::getline(_infile, line)) {
		std::vector<std::string> tokens(Tools::splitString(line));
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

	temp.insert(std::make_pair("server", &ServerBlockConfig::handleListen));
	temp.insert(std::make_pair("server_name", &ServerBlockConfig::handleServerName));

	return temp;
}

bool ServerBlockConfig::handleListen(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool ServerBlockConfig::handleServerName(const std::vector<std::string>& tokens) {	
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
		Tools::Exception("Parsing error");

	std::map<std::string, ServerBlockConfig::DirectiveHandler>::const_iterator it;
	it = _handlers.find(tokens[0]);
	if (it == _handlers.end())
		Tools::Exception("Unknown directive name");
	
	DirectiveHandler handler = it->second;
	if (!(this->*handler)(tokens))
		Tools::Exception(tokens[0] + " directive not valid");
}

