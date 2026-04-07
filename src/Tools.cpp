#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

/**
 * @brief A custom exception for the webserver.
 * @param returnCode
 * If not set, the base value is 0, and do not send a response to the client.
 * If set, it can have several purposes:
 * 1. from 100 to 599, it is used as a HTTP returnCode and send a response to the client with the corresponding reason phrase (the message that comes with the return code).
 * 2. other codes can be used for custom logics.
 *
 * @param msgLog is a log used for internal purpose / debugging only and won't be send to the client.
 */
Tools::Exception::Exception() {};
/**
 * @brief A custom exception for the webserver.
 * @param returnCode
 * If not set, the base value is 0, and do not send a response to the client.
 * If set, it can have several purposes:
 * 1. from 100 to 599, it is used as a HTTP returnCode and send a response to the client with the corresponding reason phrase (the message that comes with the return code).
 * 2. other codes can be used for custom logics.
 *
 * @param msgLog is a log used for internal purpose / debugging only and won't be send to the client.
 */
Tools::Exception::Exception(int returnCode, const std::string &msgLog) : _returnCode(returnCode), _msgLog(msgLog) {}
/**
 * @brief A custom exception for the webserver.
 * @param returnCode
 * If not set, the base value is 0, and do not send a response to the client.
 * If set, it can have several purposes:
 * 1. from 100 to 599, it is used as a HTTP returnCode and send a response to the client with the corresponding reason phrase (the message that comes with the return code).
 * 2. other codes can be used for custom logics.
 *
 * @param msgLog is a log used for internal purpose / debugging only and won't be send to the client.
 */
Tools::Exception::Exception(const std::string &msgLog) : _returnCode(0), _msgLog(msgLog) {}
Tools::Exception::~Exception() throw() {}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
Tools::Exception &Tools::Exception::operator=(const Exception &obj)
{
	if (this != &obj)
	{
		this->_msgLog = obj._msgLog;
		this->_returnCode = obj._returnCode;
	}
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Tools::Exception::getReturnCode() const { return _returnCode; }

const std::string &Tools::Exception::getMsgLog() const { return _msgLog; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

/**
 * @brief Split an input (string) according to any isspace() character
 * @returns A vector of the split string
 */
std::vector<std::string> Tools::splitString(const std::string &str)
{
	std::vector<std::string> returnVector;
	std::istringstream ss(str);
	std::string word;

	while (ss >> word)
		returnVector.push_back(word);
	return returnVector;
}

/**
 * @brief Split an `input` (string) according to a `separator` (string)
 * @returns A vector of the split string, if no separator is found, return a vector containing only the input string
 */
std::vector<std::string> Tools::splitString(const std::string &input, const std::string &separator)
{
	std::vector<std::string> returnVector;

	std::string::size_type start = 0;
	std::string::size_type pos;

	while ((pos = input.find(separator, start)) != std::string::npos)
	{
		returnVector.push_back(input.substr(start, pos - start));
		start = pos + separator.size();
	}
	returnVector.push_back(input.substr(start));
	return (returnVector);
}

/**
 * @brief Returns a reference to the last character of a string
 */
const char &Tools::getLastCharacter(const std::string &str)
{
	return *str.rbegin();
}

/**
 * @brief Removes the last character of a string
 */
void Tools::removeLastCharacter(std::string &str)
{
	if (!str.empty())
		str.resize(str.size() - 1);
}

/**
 * @brief Erases the contents of the string after the last occurrence of character `c`
 */
void Tools::eraseAfterLastCharacter(std::string &str, char c)
{
	std::string::size_type pos = str.find_last_of(c);
	if (pos != std::string::npos)
	{
		str.erase(pos + 1);
	}
}

/**
 * @brief Returns a substring from the position + 1 where the character `c` is found. If not found, returns an empty string
 * @attention Does not include char c in the result string
 */
std::string Tools::getStringAfterLastCharacter(const std::string& src, const char c) {
	std::string::size_type pos = src.find_last_of(c);
	if (pos == std::string::npos)
		return "";

	return src.substr(pos + 1);
}

/**
 * @returns A const iterator of the last element of a string
 */
const std::string::const_iterator Tools::getLastIterator(const std::string &str)
{
	if (str.empty())
		return str.end();
	return str.end() - 1;
}

/**
 * @returns A non-const iterator of the last element of a string
 */
std::string::iterator Tools::getLastIterator(std::string &str)
{
	if (str.empty())
		return str.end();
	return str.end() - 1;
}

/**
 * @returns Whether a string contains only digits (0-9)
 */
bool Tools::isNumber(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
		if (!std::isdigit(*it))
			return false;
	return true;
}

/**
 * @brief Checks whether the tokens vector has one the following formats:
 *
 * targetToken
 * 
 * {
 *
 * OR
 *
 * targetToken {
 *
 * @param targetToken Name of the directive to look for (for ex "listen")
 * @param tokens Vector of whitespace-split words from the current input line
 * @param infile Pointer to the opened config file
 *
 * @returns Whether tokens have a valid brace format
 */
bool Tools::isValidBraceFormat(const std::string &targetToken, const std::vector<std::string> &tokens, std::ifstream *infile)
{
	if (tokens[0] == targetToken)
	{
		if (tokens.size() == 1)
		{
			std::string line;
			std::getline(*infile, line);
			if (line == "{")
				return true;
		}
		else if (tokens.size() == 2 && tokens[1] == "{")
			return true;
	}
	return false;
}

/**
 * @brief Checks whether a string has a semicolon as its last character and removes it
 * @returns Whether string has a trailing semicolon
 */
bool Tools::checkAndRemoveSemicolon(std::string &str)
{
	if (Tools::getLastCharacter(str) == ';')
	{
		str.erase(Tools::getLastIterator(str));
		return true;
	}
	return false;
}

/**
 * @returns the string format of an int
 */
std::string Tools::intToString(int nbr)
{
	std::ostringstream oss;
	oss << nbr;
	return oss.str();
}

/**
 * @returns the string format of a size_t number
 */
std::string Tools::size_tToString(std::size_t nbr)
{
	std::ostringstream oss;
	oss << nbr;
	return oss.str();
}

/**
 * @returns String format of a boolean (`true` or `false`)
 */
std::string Tools::boolToString(bool b)
{
	return b ? "true" : "false";
}

/**
 * @brief Finds all occurrences of string `replaceWord` inside `input` and replaces them with `replaceBy`
 */
void Tools::findAndReplaceAllOccurrences(std::string &input, const std::string &replaceWord, const std::string &replaceBy)
{
	std::size_t pos = input.find(replaceWord);
	while (pos != std::string::npos)
	{
		input.replace(pos, replaceWord.size(), replaceBy);
		pos = input.find(replaceWord, pos + replaceBy.size());
	}
}

/**
 * @brief Checks if a string is empty or is a # comment
 * @attention Counts whitespace as empty
 */
bool Tools::lineIsEmptyOrComment(std::string &str)
{
	std::string::size_type pos = str.find_first_not_of(" \t\n\r\f\v");
	if (pos == std::string::npos)
		return true;

	return (str[pos] == '#');
}

/**
 * @brief Transforms the given string to lowercase
 */
void Tools::transformStringToLowecase(std::string &str)
{
	for (std::size_t i = 0; i < str.size(); i++)
		str[i] = std::tolower(str[i]);
}

/**
 * @returns Whether input string `str` starts with character `c`
 */
bool Tools::stringStartsWithCharacter(const std::string &str, char c)
{
	return (!str.empty() && str[0] == c);
}

/**
 * @returns Whether the given file exists
 */
bool Tools::fileExists(const char *filename)
{
	std::ifstream file(filename);
	return file.good();
}

/**
 * @brief Format: Hour:Minute:Second GMT
 */
std::string Tools::getTimeOfDay()
{
	std::time_t now = std::time(NULL);

	// Convert to UTC (GMT)
	std::tm *gmt = std::gmtime(&now);

	char buffer[100];
	std::strftime(buffer, sizeof(buffer), "%H:%M:%S GMT", gmt);

	return std::string(buffer);
}

/**
 * @brief Format: Weekday, Day Month Year Hour:Minute:Second GMT
 */
std::string Tools::getExtendedTimeOfDay()
{
	std::time_t now = std::time(NULL);

	// Convert to UTC (GMT)
	std::tm *gmt = std::gmtime(&now);

	char buffer[100];
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

	return std::string(buffer);
}

/**
 * @returns Whether the given string is a valid socket port (0 - 65535)
 */
bool Tools::isValidPort(const std::string &port)
{
	if (port.size() > 5 && !Tools::isNumber(port))
		return false;

	int portInt = std::atoi(port.c_str());
	// unsigned short max is 65535
	if (portInt <= 0 || portInt > std::numeric_limits<unsigned short>::max())
		return false;
	return true;
}

/**
 * @returns Whether the given path is a directory
 */
bool Tools::isDirectory(const char *path)
{
	struct stat info;

	if (stat(path, &info) != 0)
		return false;
	return (info.st_mode & S_IFDIR) != 0;
}

/**
 * @returns Whether the given file path is executable
 */
bool Tools::isExecutable(const char *path) {
	if (access(path, F_OK | X_OK) == 0)
		return true;
	return false;
}


/**
 * @brief if the fd is higher than -1, close it and set it to -1.
 * @return true if it closes, false if not
 */
bool Tools::closeAndResetFD(int &fd)
{
	if (fd >= 0)
	{
		close(fd);
		fd = -1;
		return true;
	}
	return false;
}

/**
 * @returns The Content-Type of a given file. If not found/valid, returns an empty string
 */
const std::string Tools::getExtension(const std::string &path)
{
	std::string::size_type pos = path.rfind(".");

	if (pos == std::string::npos || pos == 0)
		return "";

	return HttpTools::getContentType(path.substr(pos));
}

/**
 * @brief Extract the extension of a given file path
 */
const std::string Tools::extractExtension(const std::string& path) {
	std::string::size_type pos = path.rfind(".");

	return ((pos == std::string::npos || pos == 0) ? "" : path.substr(pos));
}

