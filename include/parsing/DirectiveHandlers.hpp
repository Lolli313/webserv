#pragma once

#include "LocationConfig.hpp"
#include "ConfigBase.hpp"

#include <cstdlib>
#include <limits>

class DirectiveHandlers : public ConfigBase
{
private:
	std::string _port;
	std::set<std::string> _serverNames;
	std::map<std::string, LocationConfig> _locationConfigs; // map<path, LocationConfig>
	std::ifstream& _infile;
	DirectiveHandlers();

public:
	DirectiveHandlers(std::ifstream& infile);
	~DirectiveHandlers();

	bool handleListen(const std::vector<std::string>& tokens);
	bool handleServerName(const std::vector<std::string>& tokens);
	bool handleRoot(const std::vector<std::string>& tokens);
	bool handleIndex(const std::vector<std::string>& tokens);
	bool handleAutoindex(const std::vector<std::string>& tokens);
	bool handleClientMaxBodySize(const std::vector<std::string>& tokens);
	bool handleErrorPage(const std::vector<std::string>& tokens);
	bool handleLocation(const std::vector<std::string>& tokens);
	bool handleAllowMethods(const std::vector<std::string>& tokens);
	bool handleReturn(const std::vector<std::string>& tokens);

	const std::string& getListen() const;
	const std::set<std::string>& getServerName() const;
	const std::map<std::string, LocationConfig>& getLocation() const;

	typedef	bool (DirectiveHandlers::*DirectiveHandler)(const std::vector<std::string>&);

	static const std::map<std::string, DirectiveHandler> _locationHandlers;
	static const std::map<std::string, DirectiveHandler> _initHandlers();
};
