#ifndef PARSECONFIG_HPP
#define PARSECONFIG_HPP

#include "ConfigBase.hpp"
#include "Tools.hpp"

#include <fstream>
#include <string>

class ParseConfig
{
private:
	const std::string _fileName;
	


	ParseConfig();
	ParseConfig(const ParseConfig &obj);
	ParseConfig &operator=(const ParseConfig &obj);

public:
	ParseConfig(const std::string& filePath);
	~ParseConfig();
	
};

#endif