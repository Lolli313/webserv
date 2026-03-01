#include "../include/ConfigBase.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ConfigBase::~ConfigBase() {};

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

ConfigBase& ConfigBase::operator=(const ConfigBase &obj) {
	if (this != &obj) {
		_root = obj._root;
		_index = obj._index;
		_autoindex = obj._autoindex;
		_clientMaxBodySize = obj._clientMaxBodySize;
		_errorPages = obj._errorPages;
		_allowedMethods = obj._allowedMethods;
		_returnDirective = obj._returnDirective;
	}
	return *this;
}


/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const std::string &ConfigBase::getRoot() const { return _root; }
const std::vector<std::string> &ConfigBase::getIndex() const { return _index; }
bool ConfigBase::getAutoIndex() const { return _autoindex; }
long ConfigBase::getClientMaxBodySize() const { return _clientMaxBodySize; }
const std::map<int, std::string> &ConfigBase::getErrorPages() const { return _errorPages; }
const std::set<httpMethods> &ConfigBase::getAllowMethods() const { return _allowedMethods; }
const std::pair<int, std::string> &ConfigBase::getReturnDirective() const { return _returnDirective; }

void ConfigBase::setRoot(const std::string& src){ _root = src; }
void ConfigBase::setIndex(const std::vector<std::string>& src){ _index = src; }
void ConfigBase::setAutoIndex(bool src){ _autoindex = src; }
void ConfigBase::setClientMaxBodySize(long src){ _clientMaxBodySize = src; }
void ConfigBase::setErrorPages(const std::map<int, std::string> &src){ _errorPages = src; }
void ConfigBase::setAllowMethods(const std::set<httpMethods> &src){ _allowedMethods = src; }
void ConfigBase::setReturnDirective(const std::pair<int, std::string> &src) { _returnDirective = src; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool ConfigBase::handleRoot(const std::vector<std::string>& tokens, std::ifstream& infile) {
	(void)tokens;
	(void)infile;
	return true;
}

bool ConfigBase::handleIndex(const std::vector<std::string>& tokens, std::ifstream& infile) {
	(void)tokens;
	(void)infile;
	return true;
}

bool ConfigBase::handleAutoindex(const std::vector<std::string>& tokens, std::ifstream& infile) {
	(void)tokens;
	(void)infile;
	return true;
}

bool ConfigBase::handleClientMaxBodySize(const std::vector<std::string>& tokens, std::ifstream& infile) {
	(void)tokens;
	(void)infile;
	return true;
}

bool ConfigBase::handleErrorPage(const std::vector<std::string>& tokens, std::ifstream& infile) {
	if (tokens.size() > 2)
		return handleErrorOneLiner(tokens);
	else
		return handleErrorMultiLiner(tokens, infile);
}

bool ConfigBase::handleErrorOneLiner(const std::vector<std::string>& tokens) {
	std::vector<std::string> parseTokens(tokens);
	std::string errorPagePath(parseTokens.back());
	if (!Tools::checkAndRemoveSemicolon(errorPagePath))
		return false;

	// erase first ("error_page") and last elements (errorPagePath)
	parseTokens.erase(parseTokens.end() - 1);
	parseTokens.erase(parseTokens.begin());
	
	std::map<int, std::string> errorPages;
	for (std::vector<std::string>::const_iterator it = parseTokens.begin(); it != parseTokens.end(); it++) {
		std::string temp = *it;
		if (temp.size() != 3 || !Tools::isNumber(temp))
			return false;

		int httpCode = std::atoi(temp.c_str());
		if (httpCode < 300 || httpCode > 599)
			return false;

		errorPages.insert(std::make_pair(httpCode, errorPagePath));
	}
	setErrorPages(errorPages);
	return true;
}

bool ConfigBase::handleErrorMultiLiner(const std::vector<std::string>& tokens, std::ifstream& infile) {
	if (!Tools::isValidBraceFormat("error_page", tokens, infile))
		return false;

	std::map<int, std::string> temp;
	std::string line;
	while (std::getline(infile, line)) {
		if (line.empty() || line[0] == '#')
			continue;

		std::cout << line << std::endl;
		std::vector<std::string> tokens = Tools::splitString(line);

		if (tokens[0] == "}") {
			setErrorPages(temp);
			return true;
		}

		if (tokens.size() != 2 || tokens[0].size() != 3 || !Tools::isNumber(tokens[0]))
			return false;

		int httpCode = std::atoi(tokens[0].c_str());
		if (httpCode < 300 || httpCode > 599)
			return false;

		std::string path(tokens[1]);
		if (!Tools::checkAndRemoveSemicolon(path))
			return false;

		temp.insert(std::make_pair(httpCode, path));
	}
	return false;
}

bool ConfigBase::handleAllowMethods(const std::vector<std::string>& tokens, std::ifstream& infile) {
	(void)tokens;
	(void)infile;
	return true;
}

bool ConfigBase::handleReturn(const std::vector<std::string>& tokens, std::ifstream& infile) {
	(void)tokens;
	(void)infile;
	return true;
}
