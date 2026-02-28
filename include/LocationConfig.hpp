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
	LocationConfig &operator=(const LocationConfig &obj);
		
public:
	LocationConfig(const LocationConfig &obj);
	LocationConfig(std::ifstream& infile);
	~LocationConfig();

	bool parseLocationBlock(std::ifstream& infile);
};
