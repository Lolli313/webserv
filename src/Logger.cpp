#include "Logger.hpp"
#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

/*
=================================================================
===== STATIC INITIALIZIONS  =====================================
=================================================================
*/

std::map<int, const char*>& Logger::getLevelColors() {
	static std::map<int, const char*> levelColors;
	if (levelColors.empty())
		initLevelColors(levelColors);
	return levelColors;
}

const char* Logger::getLevelColor(int level) {
	const std::map<int, const char*> &temp = getLevelColors();
	std::map<int, const char*>::const_iterator it = temp.find(level);
	return ((it == temp.end()) ? temp.find(0)->second : it->second);
}

void Logger::initLevelColors(std::map<int, const char*>& levelColors) {
	#define X(level, message, ID) levelColors[level] = message;
		LOG_LEVEL
	#undef X
}

std::map<int, std::string>& Logger::getEnumStringMap() {
	static std::map<int, std::string> enumStringMap;
	if (enumStringMap.empty())
		initEnumStringMap(enumStringMap);
	return enumStringMap;
}

const std::string& Logger::getStringFromEnum(int level) {
	const std::map<int, std::string>& temp = getEnumStringMap();
	std::map<int, std::string>::const_iterator it = temp.find(level);
	return ((it == temp.end()) ? temp.find(0)->second : it->second);
}

void Logger::initEnumStringMap(std::map<int, std::string>& enumStringMap) {
	#define X(level, message, ID) enumStringMap[level] = #level;
		LOG_LEVEL
	#undef X
}

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

void Logger::logger(LogLevel level, const std::string& message) {
	if (!(!ENABLE_INFO_LOGGING && level == INFO))
		std::clog << LIGHT_GRAY << "[" << Tools::getTimeOfDay() << "] " << getLevelColor(level) <<
				"[" << getStringFromEnum(level) << "] " << message << RESET << std::endl;
}

void Logger::logger(LogLevel level, const char* color, const std::string& message) {
	if (!(!ENABLE_INFO_LOGGING && level == INFO))
		std::clog << LIGHT_GRAY << "[" << Tools::getTimeOfDay() << "] " << color << "[" <<
				getStringFromEnum(level) << "] " << message << RESET << std::endl;
}

void Logger::logger(LogLevel level, const char* color, const std::string& key, const std::string& value) {
	if (!(!ENABLE_INFO_LOGGING && level == INFO))
		std::clog << LIGHT_GRAY << "[" << Tools::getTimeOfDay() << "] " << color << "[" <<
				getStringFromEnum(level) << "] " << key << ": " << RESET << value << std::endl;
}


