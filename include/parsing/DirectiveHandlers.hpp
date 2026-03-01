#pragma once

#include "LocationConfig.hpp"

#include <cstdlib>
#include <limits>

class DirectiveHandlers
{
private:
	std::ifstream& _infile;
	std::string _port;
	std::set<std::string> _serverNames;
	std::string _locationPath;
	LocationConfig _locationConfig;
	DirectiveHandlers();

public:
	DirectiveHandlers(std::ifstream& infile);
	~DirectiveHandlers();

	bool handleListen(const std::vector<std::string>& tokens);
	bool handleServerName(const std::vector<std::string>& tokens);
	bool handleLocation(const std::vector<std::string>& tokens);

	const std::string& getListen() const;
	const std::set<std::string>& getServerName() const;
	const std::pair<std::string, LocationConfig> getLocation() const;

	typedef	bool (DirectiveHandlers::*DirectiveHandler)(const std::vector<std::string>&);

	static const std::map<std::string, DirectiveHandler> _locationHandlers;
	static const std::map<std::string, DirectiveHandler> _initHandlers();
};
