#pragma once

#include <string>
#include <exception>

class CustomException : public std::exception
{
private:
	int _returnCode;
	std::string _msgLog;

public:
	CustomException(int returnCode, const std::string &msgLog);

	CustomException(const CustomException &obj);
	CustomException &operator=(const CustomException &obj);
	~CustomException();

	virtual const char *what() const throw() { return ""; }
};
