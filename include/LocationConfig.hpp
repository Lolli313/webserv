#pragma once

#include "ConfigBase.hpp"
#include "Tools.hpp"

#include <fstream>
#include <string>
#include <set>
#include <map>

class LocationConfig : public ConfigBase
{
private:
	LocationConfig();
	
	std::ifstream& _infile;
	
public:
	// LocationConfig(const LocationConfig &obj);
	LocationConfig(std::ifstream& infile);
	LocationConfig &operator=(const LocationConfig &obj);
	~LocationConfig();

	bool parseLocationBlock(const std::vector<std::string>& tokens);

	const LocationConfig& getLocation() const;

	typedef	bool (ConfigBase::*DirectiveHandler)(const std::vector<std::string>&, std::ifstream&);

	static const std::map<std::string, DirectiveHandler> _locationHandlers;
	static const std::map<std::string, DirectiveHandler> _initHandlers();

	void printData() const;
};
