#pragma once

#include <string>
#include <set>
#include <map>

class LocationConfig
{
private:
	std::string _root;
	std::string _index;
	bool _autoindex;
	long _clientMaxBodySize;
	std::set<std::string> _allowedMethods;
	std::map<int, std::string> _error_pages;
	std::map<int, std::string> _return;

public:
	LocationConfig();
	LocationConfig(const LocationConfig &obj);
	LocationConfig &operator=(const LocationConfig &obj);
	~LocationConfig();
};
