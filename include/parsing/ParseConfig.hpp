#ifndef PARSECONFIG_HPP
#define PARSECONFIG_HPP

#include "ServerBlockConfig.hpp"
#include "ConfigBase.hpp"
#include "Tools.hpp"

#include <fstream>
#include <string>

class ParseConfig
{
private:
	const std::string _fileName;
	std::vector<ServerBlockConfig> _serverBlockConfigVector;

	ParseConfig();
	ParseConfig(const ParseConfig &obj);
	ParseConfig &operator=(const ParseConfig &obj);

public:
	ParseConfig(const std::string& filePath);
	~ParseConfig();

	bool checkServerKeyword(const std::string& line, std::ifstream *infile);

	void printData() const;
	
	void printData() const;

};

#endif