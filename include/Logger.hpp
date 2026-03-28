#pragma once

#include "TerminalColors.hpp"
#include "HttpTools.hpp"

class Tools;

#include <iostream>
#include <string>
#include <ctime>
#include <map>

#define LOG_LEVEL					\
	X(DEBUG, GREEN_BRIGHT, 8)		\
	X(INFO, BLUE_BRIGHT, 9)			\
	X(WARNING, YELLOW_BRIGHT, 10)	\
	X(ERROR, RED_BRIGHT, 11)		\
	X(CRITICAL, RED, 12)

#define LOG Logger::logger

#define X(level, color, ID) level = ID, 
	enum LogLevel {
		LOG_LEVEL
	};
#undef X

class Logger
{
private:

	static void initLevelColors(std::map<int, const char*>& levelColors);
	static void initEnumStringMap(std::map<int, std::string>& enumStringMap);
public:

	static std::map<int, const char*>& getLevelColors();
	static const char* getLevelColor(int level);

	static std::map<int, std::string>& getEnumStringMap();
	static const std::string& getStringFromEnum(int level);

	static void logger(LogLevel level, const std::string& message);
	static void logger(httpMethods method, LogLevel level, const std::string& message);
	static void logger(LogLevel level, const char* color, const std::string& message);
	
};
