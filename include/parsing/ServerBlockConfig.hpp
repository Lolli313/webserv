#pragma once

#include "LocationConfig.hpp"
#include "ConfigBase.hpp"
#include "HttpTools.hpp"
#include "Tools.hpp"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <vector>

#define MAX_PORT_NUMBER 65535

class ServerBlockConfig : public ConfigBase
{
private:
	std::string _port;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	std::ifstream *_infile;
	ServerBlockConfig();

	/**
	 * @brief Pointer to a parsing function that matches the directive's name.
	 *
	 * Example:
	 * ```cpp
	 * &ServerBlockConfig::parseListen
	 * ```
	 */
	typedef	bool (ServerBlockConfig::*DirectiveHandler)(std::vector<std::string>&);

	static const std::map<std::string, DirectiveHandler> _serverHandlers;
	static const std::map<std::string, DirectiveHandler> _initHandlers();
	
public:
	ServerBlockConfig(std::ifstream *infile);
	ServerBlockConfig &operator=(const ServerBlockConfig &obj);
	~ServerBlockConfig();

	const std::string& getPort() const;
	const std::set<std::string>& getServerNames() const;
	const std::map<std::string, LocationConfig>& getLocationConfigs() const;

	void setPort(const std::string& src);
	void setServerNames(const std::set<std::string>& src);
	void setLocationConfigs(const std::map<std::string, LocationConfig>& src);

	bool handleStartingBrace(bool startingBraceIncluded);
	void handleDirectiveName(const std::string& line);
	bool parseListen(std::vector<std::string>& tokens);
	bool parseServerName(std::vector<std::string>& tokens);
	bool parseRoot(std::vector<std::string>& tokens);
	bool parseIndex(std::vector<std::string>& tokens);
	bool parseAutoindex(std::vector<std::string>& tokens);
	bool parseClientMaxBodySize(std::vector<std::string>& tokens);
	bool parseErrorPage(std::vector<std::string>& tokens);
	bool parseLocation(std::vector<std::string>& tokens);
	bool parseAllowMethods(std::vector<std::string>& tokens);
	bool parseReturn(std::vector<std::string>& tokens);

	void initWithDefaultData();
	void printData() const;
};
