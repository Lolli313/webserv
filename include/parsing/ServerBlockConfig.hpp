#pragma once

#include "LocationConfig.hpp"
#include "ConfigBase.hpp"
#include "Tools.hpp"

#include <iostream>
#include <fstream>
#include <vector>

class ServerBlockConfig : public ConfigBase
{
private:
	std::string _port;
	__uint8_t _duplicates;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	std::ifstream& _infile;
	ServerBlockConfig();
//	ServerBlockConfig(const ServerBlockConfig &obj);

	typedef	bool (ServerBlockConfig::*DirectiveHandler)(const std::vector<std::string>&);

	static const std::map<std::string, DirectiveHandler> _handlers;
	static const std::map<std::string, DirectiveHandler> _initHandlers();

	enum Flags {
		NONE = 0x000,
		LISTEN = 0x001,
		RETURN = 0x002
	};
	
public:
	ServerBlockConfig(std::ifstream& infile, bool startingBraceIncluded);
	ServerBlockConfig &operator=(const ServerBlockConfig &obj);
	~ServerBlockConfig();

	bool handleStartingBrace(bool startingBraceIncluded);
	void handleDirectiveName(const std::string& line);
	bool handleListen(const std::vector<std::string>& tokens);
	bool handleServerName(const std::vector<std::string>& tokens);
};
