#include "parsing/DirectiveHandlers.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

DirectiveHandlers::DirectiveHandlers(std::ifstream& infile) : _infile(infile) {}

DirectiveHandlers::~DirectiveHandlers() {}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const std::string& DirectiveHandlers::getListen() const { return _port; }

const std::set<std::string>& DirectiveHandlers::getServerName() const { return _serverNames; }

const std::map<std::string, LocationConfig>& DirectiveHandlers::getLocation() const { return _locationConfigs; }

/*
=================================================================
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

const std::map<std::string, DirectiveHandlers::DirectiveHandler> DirectiveHandlers::_locationHandlers
	= DirectiveHandlers::_initHandlers();

const std::map<std::string, DirectiveHandlers::DirectiveHandler> DirectiveHandlers::_initHandlers() {
	std::map<std::string, DirectiveHandlers::DirectiveHandler> temp;

	temp.insert(std::make_pair("root", &DirectiveHandlers::handleRoot));
	temp.insert(std::make_pair("index", &DirectiveHandlers::handleIndex));
	temp.insert(std::make_pair("autoindex", &DirectiveHandlers::handleAutoindex));
	temp.insert(std::make_pair("client_max_body_size", &DirectiveHandlers::handleClientMaxBodySize));
	temp.insert(std::make_pair("error_page", &DirectiveHandlers::handleErrorPage));
	temp.insert(std::make_pair("allow_methods", &DirectiveHandlers::handleAllowMethods));
	temp.insert(std::make_pair("return", &DirectiveHandlers::handleReturn));

	return temp;
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool DirectiveHandlers::handleListen(const std::vector<std::string>& tokens) {
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

bool DirectiveHandlers::handleServerName(const std::vector<std::string>& tokens) {
	(void)tokens;
	return false;
}

bool DirectiveHandlers::handleRoot(const std::vector<std::string>& tokens) {
	(void)tokens;
	return false;
}

bool DirectiveHandlers::handleIndex(const std::vector<std::string>& tokens) {
	(void)tokens;
	return false;
}

bool DirectiveHandlers::handleAutoindex(const std::vector<std::string>& tokens) {
	(void)tokens;
	return false;
}

bool DirectiveHandlers::handleClientMaxBodySize(const std::vector<std::string>& tokens) {
	(void)tokens;
	return false;
}

bool DirectiveHandlers::handleErrorPage(const std::vector<std::string>& tokens) {
	if (!Tools::isValidBraceFormat("error_page", tokens, _infile))
		return false;
	std::map<int, std::string> temp;
	std::string line;
	while (std::getline(_infile, line)) {
		if (line.empty() || line[0] == '#')
			continue;

		std::cout << "error page line is: " << line << std::endl;
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

bool DirectiveHandlers::handleLocation(const std::vector<std::string>& tokens) {
	(void)tokens;
	if (tokens.size() < 2 || tokens.size() > 3)
		return false;

	std::string path(tokens[1]);
	std::vector<std::string> tempTokens(tokens);
	std::vector<std::string>::iterator it = tempTokens.begin();
	std::advance(it, 1);
	tempTokens.erase(it);
	if (!Tools::isValidBraceFormat("location", tempTokens, _infile))
		return false;

	std::string line;
	while (std::getline(_infile, line)) {
		if (line.empty() || line[0] == '#')
			continue;

		std::cout << "location line is: " << line << std::endl;
		std::vector<std::string> tokens = Tools::splitString(line);
		if (tokens[0] == "}")
			return true;

		std::map<std::string, DirectiveHandlers::DirectiveHandler>::const_iterator it;
		it = _locationHandlers.find(tokens[0]);
		if (it == _locationHandlers.end())
			return false;

		DirectiveHandler handler = it->second;
		if (!(this->*handler)(tokens))
			return false;
	}
	return false;
}

bool DirectiveHandlers::handleAllowMethods(const std::vector<std::string>& tokens) {
	(void)tokens;
	return true;
}

bool DirectiveHandlers::handleReturn(const std::vector<std::string>& tokens) {
	(void)tokens;
	return false;
}

