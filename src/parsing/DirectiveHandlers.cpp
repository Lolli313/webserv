#include "parsing/DirectiveHandlers.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

DirectiveHandlers::DirectiveHandlers(std::ifstream *infile) : _infile(infile), _locationConfig(infile) {}

DirectiveHandlers::DirectiveHandlers(std::ifstream *infile, const ConfigBase &src) :
	_infile(infile),
	_locationConfig(infile, src) {}

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

const std::pair<std::string, LocationConfig> DirectiveHandlers::getLocation() const { return std::make_pair(_locationPath, _locationConfig); }

/*
=================================================================
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

const std::map<std::string, DirectiveHandlers::DirectiveHandler> DirectiveHandlers::_locationHandlers
	= DirectiveHandlers::_initHandlers();

const std::map<std::string, DirectiveHandlers::DirectiveHandler> DirectiveHandlers::_initHandlers() {
	std::map<std::string, DirectiveHandlers::DirectiveHandler> temp;

	temp.insert(std::make_pair("listen", &DirectiveHandlers::handleListen));
	temp.insert(std::make_pair("server_name", &DirectiveHandlers::handleServerName));
	temp.insert(std::make_pair("location", &DirectiveHandlers::handleLocation));

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

	// unsigned short max is 65535
	if (portStr <= 0 || portStr > std::numeric_limits<unsigned short>::max())
		return false;

	_port = Tools::intToString(portStr);
	return true;
}

bool DirectiveHandlers::handleServerName(const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		return false;

	std::vector<std::string> parseTokens(tokens);
	if (!Tools::checkAndRemoveSemicolon(parseTokens.back()))
		return false;

	parseTokens.erase(parseTokens.begin());
	_serverNames = std::set<std::string>(parseTokens.begin(), parseTokens.end());
	return true;
}

bool DirectiveHandlers::handleLocation(const std::vector<std::string>& tokens) {
	if (tokens.size() < 2 || tokens.size() > 3)
		return false;

	_locationPath = tokens[1];
	std::vector<std::string> tempTokens(tokens);
	std::vector<std::string>::iterator it = tempTokens.begin();
	std::advance(it, 1);
	tempTokens.erase(it);
	LocationConfig lc(_infile);
	if (lc.parseLocationBlock(tempTokens)) {
		_locationConfig = lc.getLocation();
		return true;
	}
	return false;
}

void DirectiveHandlers::printData() const {
	std::cout << "Location path: " << _locationPath << std::endl;
	_locationConfig.printData();
	std::cout << std::endl;
}
