#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Tools::Exception::Exception() {};
Tools::Exception::Exception(int returnCode, const std::string &msgLog) : _returnCode(returnCode), _msgLog(msgLog) {}
Tools::Exception::Exception(const std::string &msgLog) : _returnCode(0), _msgLog(msgLog) {}
// Tools::Exception::Exception(const Exception &obj) { *this = obj; };
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
std::vector<std::string> Tools::splitString(const std::string &str) {
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

	while ((pos = input.find(separator, start) != std::string::npos))
	{
		returnVector.push_back(input.substr(start, pos - start));
		start = pos + separator.size();
	}
	returnVector.push_back(input.substr(start));
	return (returnVector);
}

// Returns a reference of the last character of a string
const char& Tools::getBack(const std::string& str) {
	return str[str.length() - 1];
}

// Checks whether a string contains only digits (0-9)
bool Tools::isNumber(const std::string& str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
		if (std::isdigit(*it))
			return false;
	}
	return true;
}

/**
 * Checks whether the tokens vector has the following formats:
 * targetToken
 * OR
 * targetToken {
 */
bool Tools::isValidBraceFormat(const std::string& targetToken, const std::vector<std::string>& tokens) {
	bool validFormat =
		(tokens.size() == 1 && tokens[0] == targetToken) ||
		(tokens.size() == 2 && tokens[1] == "{");

	return validFormat;
}


