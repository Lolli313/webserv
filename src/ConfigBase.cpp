#include "../include/ConfigBase.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ConfigBase::~ConfigBase() {};

ConfigBase::ConfigBase(const ConfigBase &obj) { *this = obj; }

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

ConfigBase &ConfigBase::operator=(const ConfigBase &obj)
{
	if (this != &obj)
	{
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
const std::set<std::string> &ConfigBase::getAllowMethods() const { return _allowedMethods; }
const std::pair<int, std::string> &ConfigBase::getReturnDirective() const { return _returnDirective; }

void ConfigBase::setRoot(const std::string &src) { _root = src; }
void ConfigBase::setIndex(const std::vector<std::string> &src) { _index = src; }
void ConfigBase::setAutoIndex(bool src) { _autoindex = src; }
void ConfigBase::setClientMaxBodySize(long src) { _clientMaxBodySize = src; }
void ConfigBase::setErrorPages(const std::map<int, std::string> &src) { _errorPages = src; }
void ConfigBase::setAllowMethods(const std::set<std::string> &src) { _allowedMethods = src; }
void ConfigBase::setReturnDirective(const std::pair<int, std::string> &src) { _returnDirective = src; }

/*
=================================================================
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

const bitmask_t ConfigBase::_allowedBits;

const UnitConversion ConfigBase::_conversionTable[] = {
	{MASK_K, MAX_K, FACTOR_K, MAX_STR_K},
	{MASK_M, MAX_M, FACTOR_M, MAX_STR_M},
	{MASK_G, MAX_G, FACTOR_G, MAX_STR_G}};

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool ConfigBase::handleRoot(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	(void)infile;
	if (tokens.size() != 2)
		return false;

	std::vector<std::string> parseTokens(tokens);
	if (!Tools::checkAndRemoveSemicolon(parseTokens[1]))
		return false;

	parseTokens.erase(parseTokens.begin());
	setRoot(parseTokens[0]);
	return true;
}

bool ConfigBase::handleIndex(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	(void)infile;
	if (tokens.size() < 2)
		return false;

	std::vector<std::string> parseTokens(tokens);
	if (!Tools::checkAndRemoveSemicolon(parseTokens.back()))
		return false;

	parseTokens.erase(parseTokens.begin());
	setIndex(parseTokens);
	return true;
}

bool ConfigBase::handleAutoindex(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	(void)infile;
	if (tokens.size() != 2)
		return false;

	std::vector<std::string> parseTokens(tokens);
	if (!Tools::checkAndRemoveSemicolon(parseTokens[1]))
		return false;

	std::string autoindexArgument(parseTokens[1]);
	if (autoindexArgument == "on")
		setAutoIndex(true);
	else if (autoindexArgument == "off")
		setAutoIndex(false);
	else
		return false;

	return true;
}

bool ConfigBase::handleClientMaxBodySize(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	(void)infile;
	if (tokens.size() != 2)
		return false;

	std::vector<std::string> parseTokens(tokens);
	if (!Tools::checkAndRemoveSemicolon(parseTokens[1]))
		return false;

	std::string maxSize(parseTokens[1]);
	return handleMaxSizeConversion(maxSize);
}

bool ConfigBase::handleMaxSizeConversion(std::string &maxSize)
{
	long resultMaxSize;
	if (!Tools::isNumber(maxSize))
	{
		const std::string digits("0123456789");
		std::size_t pos = maxSize.find_first_not_of(digits);
		if (pos == std::string::npos)
		{
			std::clog << "Unknown error" << std::endl;
			return false;
		}
		bitmask_t foundBit = charToBit(maxSize[pos]);
		if (!(foundBit & _allowedBits))
			return false; // character was not k, m, g

		resultMaxSize = expandMaskedString(maxSize, foundBit);
	}
	else
	{
		if (maxSize.size() > 10)
			throw Tools::Exception("clientMaxBodySize value too big");
		resultMaxSize = std::atol(maxSize.c_str());
		if (resultMaxSize > std::numeric_limits<unsigned int>::max())
			throw Tools::Exception("clientMaxBodySize value too big");
	}
	setClientMaxBodySize(resultMaxSize);
	return true;
}

/**
 * @brief Converts a masked string (e.g., "100M") into a byte value.
 * * This function identifies the unit suffix using bitwise logic, validates
 * the numeric string against length and value constraints to prevent
 * integer overflow, and returns the expanded value in bytes.
 *
 * @param src The string to convert. The suffix character will be erased.
 * @param foundBit The bitmask representing the detected suffix (K, M, or G).
 * @return The converted value as an unsigned int, or 0 if no mask matches.
 * @throws Tools::Exception if the string is too long or the value exceeds limits.
 */
unsigned int ConfigBase::expandMaskedString(std::string &src, bitmask_t foundBit)
{
	bitmask_t activeBit = foundBit & _allowedBits;
	const std::size_t numUnits = sizeof(_conversionTable) / sizeof(_conversionTable[0]);
	for (std::size_t i = 0; i < numUnits; i++)
	{
		if (activeBit == _conversionTable[i].mask)
		{
			src.erase(Tools::getLastIterator(src));
			if (src.size() > _conversionTable[i].maxStrLen)
				throw Tools::Exception("clientMaxBodySize value too big");

			unsigned int val = static_cast<unsigned int>(std::atol(src.c_str()));
			if (val > _conversionTable[i].maxVal)
				throw Tools::Exception("clientMaxBodySize value too big");

			return val * _conversionTable[i].factor;
		}
	}
	return 0;
}

bitmask_t ConfigBase::charToBit(char c)
{

	// Allow uppercase as well lowercase characters
	c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	switch (c)
	{
	case 'k':
		return MASK_K;
	case 'm':
		return MASK_M;
	case 'g':
		return MASK_G;
	default:
		return 0;
	}
}

bool ConfigBase::handleErrorPage(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	if (tokens.size() > 2)
		return handleErrorOneLiner(tokens);
	else
		return handleErrorMultiLiner(tokens, infile);
}

bool ConfigBase::handleErrorOneLiner(const std::vector<std::string> &tokens)
{
	std::vector<std::string> parseTokens(tokens);
	std::string errorPagePath(parseTokens.back());
	if (!Tools::checkAndRemoveSemicolon(errorPagePath))
		return false;

	// erase first ("error_page") and last elements (errorPagePath)
	parseTokens.erase(parseTokens.end() - 1);
	parseTokens.erase(parseTokens.begin());

	std::map<int, std::string> errorPages;
	for (std::vector<std::string>::const_iterator it = parseTokens.begin(); it != parseTokens.end(); it++)
	{
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

bool ConfigBase::handleErrorMultiLiner(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	if (!Tools::isValidBraceFormat("error_page", tokens, infile))
		return false;

	std::map<int, std::string> temp;
	std::string line;
	while (std::getline(infile, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		std::cout << line << std::endl;
		std::vector<std::string> tokens = Tools::splitString(line);

		if (tokens[0] == "}")
		{
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

bool ConfigBase::handleAllowMethods(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	(void)infile;
	if (tokens.size() < 2)
		return false;

	std::vector<std::string> parseTokens(tokens);
	if (!Tools::checkAndRemoveSemicolon(parseTokens.back()))
		return false;

	parseTokens.erase(parseTokens.begin());
	std::set<std::string> allowedMethods;
	std::vector<std::string>::iterator it = parseTokens.begin();
	for (; it != parseTokens.end(); it++)
	{
		if (!HttpTools::isValidMethod(*it))
			return false;
		allowedMethods.insert(*it);
	}
	setAllowMethods(allowedMethods);
	return true;
}

bool ConfigBase::handleReturn(const std::vector<std::string> &tokens, std::ifstream &infile)
{
	(void)infile;
	if (tokens.size() < 2 || tokens.size() > 3)
		return false;

	std::vector<std::string> parseTokens(tokens);
	if (!Tools::checkAndRemoveSemicolon(parseTokens.back()))
		return false;

	parseTokens.erase(parseTokens.begin());
	std::string temp(parseTokens[0]);
	if (temp.size() != 3 || !Tools::isNumber(temp))
		return false;

	int httpCode = std::atoi(temp.c_str());
	if (httpCode < 300 || httpCode > 599)
		return false;

	std::string path("");
	if (parseTokens.size() > 1)
		path = parseTokens[1];
	setReturnDirective(std::make_pair(httpCode, path));
	return true;
}
