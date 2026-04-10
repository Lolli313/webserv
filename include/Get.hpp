#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "ConfigBase.hpp"
#include "Polling.hpp"
#include "Tools.hpp"

#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <fcntl.h>
#include <string>

struct Directory
{
	std::string name;
	std::string type;
	std::string date;
	std::string size;
};

class Get
{
private:
	int _fd;
	const HttpRequest &_request;
	const ConfigBase *_config;
	std::string _host;
	std::string _file;
	std::string _path;
	bool _autoindex;
	Get(const HttpRequest &request, const ConfigBase *config);

public:
	// Get(const Get &obj);
	// Get &operator=(const Get &obj);
	~Get();

	const std::string &getPath() const;
	void checkRequest();
	void checkAndSetFile(const std::string &path);
	bool setIndexFile(const std::string &path);
	bool handleIndexFile();
	void closeAndResetFD();
	const std::vector<Directory> handleAutoindex(const std::string &path) const;
	const std::string autoIndexToJson(const std::vector<Directory> &currDir) const;
	bool isCgiLocation(const std::string& path);
	bool handleCgiPage();
	const std::string& prepareResponse(HttpResponse& response);

	static const std::string executeGet(const HttpRequest &request, const ConfigBase *config);
};
