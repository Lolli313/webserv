#pragma once

#include "ConfigBase.hpp"

#include <string>
#include <set>
#include <map>

class LocationConfig : public ConfigBase
{
private:
	

public:
	LocationConfig();
	LocationConfig(const LocationConfig &obj);
	LocationConfig &operator=(const LocationConfig &obj);
	~LocationConfig();
};
