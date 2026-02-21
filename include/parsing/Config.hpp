#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Tools.hpp"

#include <fstream>
#include <string>

class Config
{
private:
	const std::string _fileName;


	Config();
	Config(const Config &obj);
	Config &operator=(const Config &obj);

public:
	Config(const std::string& filePath);
	~Config();
	
};

#endif