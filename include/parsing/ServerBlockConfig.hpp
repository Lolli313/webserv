#pragma once

#include "DirectiveHandlers.hpp"
#include "LocationConfig.hpp"
#include "ConfigBase.hpp"
#include "HttpTools.hpp"
#include "Tools.hpp"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <vector>

#define MAX_PORT_NUMBER 65535

class ServerBlockConfig : public ConfigBase
{
private:
	std::string _port;
//	__uint8_t _duplicates;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	std::ifstream& _infile;
	ServerBlockConfig();
//	ServerBlockConfig(const ServerBlockConfig &obj);

	typedef	bool (ServerBlockConfig::*DirectiveHandler)(const std::vector<std::string>&);

	static const std::map<std::string, DirectiveHandler> _serverHandlers;
	static const std::map<std::string, DirectiveHandler> _initHandlers();

	// enum Flags {
	// 	NONE = 0x000,
	// 	LISTEN = 0x001,
	// 	RETURN = 0x002
	// };
	
public:
	ServerBlockConfig(std::ifstream& infile);
	ServerBlockConfig &operator=(const ServerBlockConfig &obj);
	~ServerBlockConfig();

	bool handleStartingBrace(bool startingBraceIncluded);
	void handleDirectiveName(const std::string& line);
	bool parseListen(const std::vector<std::string>& tokens);
	bool parseServerName(const std::vector<std::string>& tokens);
	bool parseRoot(const std::vector<std::string>& tokens);
	bool parseIndex(const std::vector<std::string>& tokens);
	bool parseAutoindex(const std::vector<std::string>& tokens);
	bool parseClientMaxBodySize(const std::vector<std::string>& tokens);
	bool parseErrorPage(const std::vector<std::string>& tokens);
	bool parseLocation(const std::vector<std::string>& tokens);
	bool parseAllowMethods(const std::vector<std::string>& tokens);
	bool parseReturn(const std::vector<std::string>& tokens);

	void printData() const;
};
