#pragma once

#include <string>
#include <exception>

class Tools {
/* Tools Orthodox Canonical Form is in private so that it cannot be used from outside of this class
   since it shouldn't be used anywhere */
private:
	Tools();
	Tools(const Tools& src);
	Tools& operator=(const Tools& src);
	~Tools();

public:
	class CustomException : public std::exception {
	private:
		int _returnCode;
		std::string _msgLog;

	public:
		CustomException(int returnCode, const std::string &msgLog);
		CustomException(const std::string &msgLog);
		CustomException(const CustomException &obj);
		CustomException &operator=(const CustomException &obj);
		virtual ~CustomException() throw(); /* the destructor of std::exception has the throw() specifier
		which means that the destructor of the child class has to match its parent's "danger level" */

		virtual const char *what() const throw() { return _msgLog.c_str(); }
	};

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
