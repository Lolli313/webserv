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

// Split an input (string) according to any isspace() character
std::vector<std::string> Tools::splitString(const std::string &str)
{
	std::vector<std::string> returnVector;
	std::istringstream ss(str);
	std::string word;

	while (ss >> word)
		returnVector.push_back(word);
	return returnVector;
}

// Split an input (string) according to a separator (string)
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

// Returns a reference of the last character of a string
const char &Tools::getLastCharacter(const std::string &str)
{
	return *str.rbegin();
}

void Tools::removeLastCharacter(std::string& str) {
	if (!str.empty())
		str.resize(str.size() - 1);
}

void Tools::eraseAfterLastCharacter(std::string& str, char c) {
	std::string::size_type pos = str.find_last_of(c);
	if (pos != std::string::npos) {
		str.erase(pos + 1);
	}
}


const std::string::const_iterator Tools::getLastIterator(const std::string &str)
{
	if (str.empty())
		return str.end();
	return str.end() - 1;
}

std::string::iterator Tools::getLastIterator(std::string &str)
{
	if (str.empty())
		return str.end();
	return str.end() - 1;
}

// Checks whether a string contains only digits (0-9)
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

bool Tools::checkAndRemoveSemicolon(std::string &str)
{
	if (Tools::getLastCharacter(str) == ';')
	{
		str.erase(Tools::getLastIterator(str));
		return true;
	}
	return false;
}

std::string Tools::intToString(int nbr)
{
	std::ostringstream oss;
	oss << nbr;
	return oss.str();
}

std::string Tools::boolToString(bool b) {
	return b ? "true" : "false";
}


void Tools::findAndReplaceAllOccurences(std::string& input, const std::string& replaceWord, const std::string& replaceBy) {
	std::size_t pos = input.find(replaceWord);
	while (pos != std::string::npos) {
		input.replace(pos, replaceWord.size(), replaceBy);
		pos = input.find(replaceWord, pos + replaceBy.size());
	}
}

/**
 * @brief Checks if a string is empty or is a # comment
 * @attention Ignores whitespace
 */
bool Tools::lineIsEmptyOrComment(std::string& str) {
	std::string::size_type pos = str.find_first_not_of(" \t\n\r\f\v");
	if (pos == std::string::npos)
		return true;
	
	return (str[pos] == '#');
}

void Tools::transformStringToLowecase(std::string& str) {
	for (std::size_t i = 0; i < str.size(); i++)
		str[i] = std::tolower(str[i]);
}

bool Tools::stringStartsWithCharacter(const std::string& str, char c) {
	return (!str.empty() && str[0] == c);
}

bool Tools::fileExists(const char* filename) {
    std::ifstream file(filename);
    return file.good();
}

std::string Tools::getTimeOfDay() {
	std::time_t now = std::time(NULL);

    // Convert to UTC (GMT)
    std::tm *gmt = std::gmtime(&now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S GMT", gmt);

	return std::string(buffer);
}

std::string Tools::getExtendedTimeOfDay() {
	std::time_t now = std::time(NULL);

    // Convert to UTC (GMT)
    std::tm *gmt = std::gmtime(&now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

	return std::string(buffer);
}
