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
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	std::ifstream& _infile;
	ServerBlockConfig();
//	ServerBlockConfig(const ServerBlockConfig &obj);
	
public:
	ServerBlockConfig(std::ifstream& infile, bool startingBraceIncluded);
	ServerBlockConfig &operator=(const ServerBlockConfig &obj);
	~ServerBlockConfig();

	bool handleStartingBrace(bool startingBraceIncluded);
};
