#include "../include/ConfigBase.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/


// Initializes the object with default values
ConfigBase::ConfigBase() {
	initWithDefaultData();
}

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
void ConfigBase::setIndex(std::vector<std::string>::const_iterator start, std::vector<std::string>::const_iterator end) {
	_index.assign(start, end);
}
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

// See ValidChars for explanation
const bitmask_t ConfigBase::_allowedBits;

/**
 * @brief Initialize each ValidChar with their corresponding string to int
 * helper parameters
 */
const UnitConversion ConfigBase::_conversionTable[] = {
	{MASK_K, MAX_K, FACTOR_K, MAX_STR_K},
	{MASK_M, MAX_M, FACTOR_M, MAX_STR_M},
	{MASK_G, MAX_G, FACTOR_G, MAX_STR_G}};

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool ConfigBase::handleRoot(std::vector<std::string> &tokens, std::ifstream *infile)
{
	(void)infile;
	if (tokens.size() != 2)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens[1]))
		return false;

	std::string& path = tokens[1];
	if (path.empty() || !Tools::stringStartsWithCharacter(path, '/'))
		return false;
	setRoot(path);
	return true;
}

bool ConfigBase::handleIndex(std::vector<std::string> &tokens, std::ifstream *infile)
{
	(void)infile;
	if (tokens.size() < 2)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens.back()))
		return false;

	setIndex(tokens.begin() + 1, tokens.end());
	return true;
}

bool ConfigBase::handleAutoindex(std::vector<std::string> &tokens, std::ifstream *infile)
{
	(void)infile;
	if (tokens.size() != 2)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens[1]))
		return false;

	const std::string& autoindexArgument(tokens[1]);
	if (autoindexArgument == "on")
		setAutoIndex(true);
	else if (autoindexArgument == "off")
		setAutoIndex(false);
	else
		return false;

	return true;
}

bool ConfigBase::handleClientMaxBodySize(std::vector<std::string> &tokens, std::ifstream *infile)
{
	(void)infile;
	if (tokens.size() != 2)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens[1]))
		return false;

	std::string& maxSize(tokens[1]);
	return handleMaxSizeConversion(maxSize);
}

bool ConfigBase::handleMaxSizeConversion(std::string &maxSize)
{
	long resultMaxSize;
	if (!Tools::isNumber(maxSize)) // numeric string has a character
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
	else // string is purely a number, no kilo, mega or giga conversion needed
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

/**
 * @brief Converts a suffix character to its corresponding bitmask.
 * Maps 'k', 'm', or 'g' (case-insensitive) to their respective @ref ValidChars 
 * flag for use in bitwise validation.
 * @param c The character to convert (e.g., 'K', 'm', 'G').
 * @return bitmask_t The matching @ref ValidChars flag, or `NONE` if the character is invalid.
 * @note This function performs an internal `std::tolower` conversion to ensure
 * case-insensitivity.
 */
bitmask_t ConfigBase::charToBit(char c)
{
	c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	switch (c)
	{
	case 'k': return MASK_K;
	case 'm': return MASK_M;
	case 'g': return MASK_G;
	default: return NONE;
	}
}

bool ConfigBase::handleErrorPage(std::vector<std::string> &tokens, std::ifstream *infile)
{
	if (tokens.size() > 2)
		return handleErrorOneLiner(tokens);
	else
		return handleErrorMultiLiner(tokens, infile);
}

/**
 * @brief If the error_page directive is formatted on one line, match each HTTP code
 * with the same path
 * 
 * (for e.g```error_page 400 404 /errors/api_error.json;```)
 */
bool ConfigBase::handleErrorOneLiner(std::vector<std::string> &tokens)
{
	std::string& errorPagePath(tokens.back());
	if (!Tools::checkAndRemoveSemicolon(errorPagePath) || errorPagePath.empty()
		|| !Tools::stringStartsWithCharacter(errorPagePath, '/'))
		return false;

	// Ignore the first token "error_page"
	std::vector<std::string>::const_iterator it = tokens.begin() + 1;
	// End iterator to ignore the error path
	std::vector<std::string>::const_iterator eit = tokens.end() - 1;

	for (; it != eit; it++)
	{
		const std::string& temp = *it;
		if (temp.size() != 3 || !Tools::isNumber(temp))
			return false;

		int httpCode = std::atoi(temp.c_str());
		if (httpCode < 300 || httpCode > 599)
			return false;

		addOrReplaceErrorPage(httpCode, errorPagePath);
	}
	return true;
}

/**
 * @brief If the error_page directive is formatted as a block, match each HTTP code
 * with their own path
 * 
 * for e.g
 * ```
 * error_page {
		403 /errors/403.html;
		404 /errors/404.html;
		}
	```
 */
bool ConfigBase::handleErrorMultiLiner(std::vector<std::string> &tokens, std::ifstream *infile)
{
	if (!Tools::isValidBraceFormat("error_page", tokens, infile))
		return false;

	std::string line;
	while (std::getline(*infile, line))
	{
		if (Tools::lineIsEmptyOrComment(line))
			continue;

		tokens = Tools::splitString(line);

		if (tokens[0] == "}")
			return true;

		if (tokens.size() != 2 || tokens[0].size() != 3 || !Tools::isNumber(tokens[0]))
			return false;

		int httpCode = std::atoi(tokens[0].c_str());
		if (httpCode < 300 || httpCode > 599)
			return false;

		std::string& errorPagePath(tokens[1]);
		if (!Tools::checkAndRemoveSemicolon(errorPagePath) || errorPagePath.empty()
		|| !Tools::stringStartsWithCharacter(errorPagePath, '/'))
			return false;

		addOrReplaceErrorPage(httpCode, errorPagePath);
	}
	return false;
}

/**
 * @brief Updates or adds a custom error page for a specific HTTP status code.
 *
 * If a path already exists for the given @p httpCode, it is overwritten with 
 * the new @p errorPagePath.
 *
 * @param httpCode The HTTP status code (e.g., 404, 500).
 * @param errorPagePath The file system path to the custom HTML error page.
 */
void ConfigBase::addOrReplaceErrorPage(int httpCode, const std::string& errorPagePath) {
	_errorPages[httpCode] = errorPagePath;
}

bool ConfigBase::handleAllowMethods(std::vector<std::string> &tokens, std::ifstream *infile)
{
	(void)infile;
	if (tokens.size() < 2)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens.back()))
		return false;

	std::set<std::string> allowedMethods;
	std::vector<std::string>::iterator it = tokens.begin() + 1;
	for (; it != tokens.end(); it++)
	{
		if (!HttpTools::isValidMethod(*it))
			return false;
		allowedMethods.insert(*it);
	}
	setAllowMethods(allowedMethods);
	return true;
}

bool ConfigBase::handleReturn(std::vector<std::string> &tokens, std::ifstream *infile)
{
	(void)infile;
	if (tokens.size() < 2 || tokens.size() > 3)
		return false;

	if (!Tools::checkAndRemoveSemicolon(tokens.back()))
		return false;

	std::string& temp(tokens[1]);
	if (temp.size() != 3 || !Tools::isNumber(temp))
		return false;

	int httpCode = std::atoi(temp.c_str());
	if (httpCode < 300 || httpCode > 599)
		return false;

	std::string path("");
	if (tokens.size() > 2)
		path = tokens[2];
	setReturnDirective(std::make_pair(httpCode, path));
	return true;
}

void ConfigBase::printData() const {
	std::clog << "Root: " << getRoot() << std::endl;

	std::clog << "Indexes: ";
	std::vector<std::string>::const_iterator indexit = getIndex().begin();
	for (;indexit != getIndex().end(); indexit++) {
		std::clog << *indexit << ", ";
	}
	std::clog << std::endl;
	
	std::clog << "autoindex: " << getAutoIndex() << std::endl;

	std::clog << "clientMaxBodySize: " << getClientMaxBodySize() << std::endl;

	std::clog << "error pages: ";
	if (getErrorPages().empty()) {}
		std::clog << std::endl;
	std::map<int, std::string>::const_iterator errorit = getErrorPages().begin();
	for (;errorit != getErrorPages().end(); errorit++) {
		std::clog << errorit->first << ", " << errorit->second << std::endl;
	}

	std::clog << "allow methods: ";
	std::set<std::string>::const_iterator allowMethodit = getAllowMethods().begin();
	for (;allowMethodit != getAllowMethods().end(); allowMethodit++) {
		std::clog << *allowMethodit << ", ";
	}
	std::clog << std::endl;

	std::clog << "return directive: ";
	std::clog << getReturnDirective().first << ", ";
	std::clog << ((getReturnDirective().second.empty()) ? "\"\"" : getReturnDirective().second);
	std::clog << std::endl;
}

/**
 * @brief initializes the ConfigBase object with harcoded default values
 * 
 * ```
 * **Root** | PWD/files
 * **Index** | index.html
 * **Auto Index** | false
 * **Client Max Body Size** | 5M
 * **Error Pages** | *empty*
 * **Allowed Methods** | GET
 * **Return Directive** | *empty*
 * ```
 */
void ConfigBase::initWithDefaultData() {
	initRoot();
	_index.push_back("index.html");
	setAutoIndex(false);
	std::string temp(DEFAULT_CLIENT_MAX_BODY_SIZE);
	setClientMaxBodySize(expandMaskedString(temp, MASK_M));
	_allowedMethods.insert("GET");
	setReturnDirective(std::make_pair(0, ""));
}

void ConfigBase::initRoot() {
	const char* path = std::getenv("PWD");
	if (!path)
		setRoot(LAST_RESORT_PATH);
	else {
		std::string root(path);
		root += FILE_FOLDER_NAME;
		setRoot(root);
	}
}
