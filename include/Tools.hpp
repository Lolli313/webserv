#pragma once

#include "Logger.hpp"

class Logger;

#include <sys/stat.h>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>
#include <ctime>

#define BUFFERSIZE 4096

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
	class Exception : public std::exception
	{
	private:
		int _returnCode; // If not defined, return code = 0
		std::string _msgLog;

		Exception();
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
	static const char &getLastCharacter(const std::string &str);
	static void removeLastCharacter(std::string &str);
	static void eraseAfterLastCharacter(std::string &str, char c);
	static std::string getStringAfterLastCharacter(const std::string& src, const char c);
	static const std::string::const_iterator getLastIterator(const std::string &str);
	static std::string::iterator getLastIterator(std::string &str);
	static bool isNumber(const std::string &str);
	static bool isValidBraceFormat(const std::string &targetToken, const std::vector<std::string> &tokens, std::ifstream *infile);
	static bool checkAndRemoveSemicolon(std::string &str);
	static std::string intToString(int nbr);
	static std::string size_tToString(std::size_t nbr);
	static std::string boolToString(bool b);
	static bool checkPath(const std::string &path);
	static void findAndReplaceAllOccurrences(std::string &input, const std::string &replaceWord, const std::string &replaceBy);
	static bool lineIsEmptyOrComment(std::string &str);
	static void transformStringToLowecase(std::string &str);
	static bool stringStartsWithCharacter(const std::string &str, char c);
	static bool fileExists(const char *filename);
	static std::string getTimeOfDay();
	static std::string getExtendedTimeOfDay();
	static bool isValidPort(const std::string &port);
	static bool isDirectory(const char *path);
	static bool isExecutable(const char *path);
	static bool closeAndResetFD(int &fd);
	static const std::string getExtension(const std::string &path);
	static const std::string extractExtension(const std::string& path);
};
