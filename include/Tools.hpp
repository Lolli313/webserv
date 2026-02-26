#pragma once

#include <exception>
#include <sstream>
#include <string>
#include <vector>

class Tools
{
	/* Tools Orthodox Canonical Form is in private so that it cannot be used from outside of this class
	   since it shouldn't be used anywhere */
private:
	Tools();
	Tools(const Tools &src);
	Tools &operator=(const Tools &src);
	~Tools();

public:
	class Exception : public std::exception
	{
	private:
		int _returnCode; // If not defined, return code = 0
		std::string _msgLog;

		Exception();
		//		Exception(const Exception &obj);
		Exception &operator=(const Exception &obj);

	public:
		Exception(int returnCode, const std::string &msgLog);
		Exception(const std::string &msgLog);
		virtual ~Exception() throw(); /* the destructor of std::exception has the throw() specifier
		which means that the destructor of the child class has to match its parent's "danger level" */

		virtual const char *what() const throw() { return _msgLog.c_str(); }

		int getReturnCode() const;
		const std::string &getMsgLog() const;
	};

	// ========================================================================================================
	// PARSING TOOLS ==========================================================================================
	// ========================================================================================================
	
	static std::vector<std::string> splitString(const std::string &str);
	static std::vector<std::string> splitString(const std::string &str, const std::string &separator);

	// class HTTPErrorException : public std::exception {
	// private:
	// 	int _HTTPErrorCode;
	// 	std::string _errorMessage;

	// public:
	// 	HTTPErrorException(int HTTPErrorCode, const std::string &errorMessage);

	// 	HTTPErrorException(const HTTPErrorException &obj);
	// 	HTTPErrorException &operator=(const HTTPErrorException &obj);
	// 	~HTTPErrorException();

	// 	int getHTTPErrorCode() const;

	// 	virtual const char *what() const throw() { return _errorMessage.c_str(); }
	// };
};
