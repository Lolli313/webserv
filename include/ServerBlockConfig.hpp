#pragma once

#include "LocationConfig.hpp"
#include "ConfigBase.hpp"

#include <iostream>

class ServerBlockConfig : public ConfigBase
{
private:
	std::string _port;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>

public:
	ServerBlockConfig();
	ServerBlockConfig(std::ifstream& infile, bool startingBraceIncluded);
	ServerBlockConfig(const ServerBlockConfig &obj);
	ServerBlockConfig &operator=(const ServerBlockConfig &obj);
	~ServerBlockConfig();
};
