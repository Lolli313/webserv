#pragma once

#include "HttpTools.hpp"

#include <string>
#include <map>
#include <set>

class ConfigInterface
{
private:
	std::string _root;
	std::string _index;
	bool _autoindex;
	long clientMaxBodySize;
	std::map<int, std::string> _errorPages;
	std::set<httpMethods> _allowMethods;


public:
	ConfigInterface();
	ConfigInterface(const ConfigInterface &obj);
	ConfigInterface &operator=(const ConfigInterface &obj);
	~ConfigInterface();
};
