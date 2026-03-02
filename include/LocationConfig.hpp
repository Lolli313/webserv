#pragma once

#include "ConfigBase.hpp"
#include "Tools.hpp"
#include "Server.hpp"

#include <fstream>
#include <string>
#include <set>
#include <map>

class LocationConfig : public ConfigBase
{
private:
	std::ifstream &_infile;
	LocationConfig();

public:
	// LocationConfig(const LocationConfig &obj);
	LocationConfig(const ConfigBase &obj, std::ifstream &infile);
	LocationConfig(std::ifstream &infile);
	LocationConfig &operator=(const LocationConfig &obj);
	LocationConfig &operator=(const Server &obj);
	~LocationConfig();

	bool parseLocationBlock(const std::vector<std::string> &tokens);

	const LocationConfig &getLocation() const;

	typedef bool (ConfigBase::*DirectiveHandler)(const std::vector<std::string> &, std::ifstream &);

	static const std::map<std::string, DirectiveHandler> _locationHandlers;
	static const std::map<std::string, DirectiveHandler> _initHandlers();
};
