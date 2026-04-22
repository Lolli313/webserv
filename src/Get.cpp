#include "Get.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Get::Get(const HttpRequest &request, const ConfigBase *config) : _fd(-1), _request(request), _config(config), _autoindex(false) {}

Get::~Get() { Tools::closeAndResetFD(_fd); }


/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const std::string &Get::getPath() const { return _path; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

void Get::checkRequest()
{
	LOG(INFO, YELLOW_BRIGHT, "checkRequest");
	std::map<std::string, std::string>::const_iterator it = _request.getHeader().find("host");
	if (it == _request.getHeader().end())
		throw Tools::Exception(400, "GET: host not found.");
	_host = it->second;
}

int openFile(const std::string &path)
{
	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		if (errno == ENOENT)
			throw Tools::Exception(404, "GET: file not found");
		else if (errno == EACCES)
			throw Tools::Exception(403, "GET: not permitted");
		else
			throw Tools::Exception(500, "GET: unknown error, cannot open the file");
	}
	return fd;
}

bool Get::setIndexFile(const std::string &path)
{
	LOG(DEBUG, "setIndexFile");
	for (std::vector<std::string>::const_iterator it = _config->getIndex().begin(); it != _config->getIndex().end(); it++)
	{
		std::string currIndex = path + *it;
		if (Tools::fileExists(currIndex.c_str()))
		{
			_path.append(*it);
			return true;
		}
	}
	return false;
}

const std::string Get::autoIndexToJson(const std::vector<Directory> &currDir) const
{
	LOG(DEBUG, "autoIndexToJson");
	std::string output("[\n");
	std::ostringstream oss;

	for (std::vector<Directory>::const_iterator it = currDir.begin(); it != currDir.end(); it++)
	{
		oss << "{\n";

		oss << "	\"name\": \"" << it->name << "\",\n"
			<< "	\"type\": \"" << it->type << "\",\n"
			<< "	\"size\": \"" << it->size << "\",\n"
			<< "	\"mtime\": \"" << it->date << "\"\n"
			<< "}";
		if (it != currDir.end() - 1)
			oss << ",\n";
		else
			oss << "\n";
	}
	output.append(oss.str());
	output.append("]");
	return output;
}

const std::vector<Directory> Get::handleAutoindex(const std::string &path) const
{
	LOG(DEBUG, "handleAutoIndex");
	std::vector<Directory> dirVector;
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		throw Tools::Exception(500, "GET: handleAutoIndex cannot open requested directory (opendir)");

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		Directory dirObject;

		dirObject.name = entry->d_name;
		if (dirObject.name == "." || dirObject.name == "..")
			continue;

		std::string fullPath = path + "/" + dirObject.name;

		struct stat info;
		if (stat(fullPath.c_str(), &info) != 0)
		{
			LOG(ERROR, "GET: handleAutoIndex stat failed for " + fullPath);
			continue;
		}

		dirObject.type = S_ISDIR(info.st_mode) ? "directory" : "file";

		dirObject.size = Tools::size_tToString(info.st_size);

		char timeBuff[64];
		std::strftime(timeBuff, sizeof(timeBuff), "%d-%b-%Y %H:%M",
					  std::localtime(&info.st_mtime));
		dirObject.date = timeBuff;

		dirVector.push_back(dirObject);
	}
	closedir(dir);
	return dirVector;
}

/**
 * @brief First check if the config file has an index for the given path, then check if a index.html exists in the given path,
 * 	then check and handle autoindex, and if none of the previous, throws an error.
 * @return false if the _fd is set and thus need to be open following the normal logic.
 * 	True if the _file is already being handled by the function and it doesn't have to follow the normal logic.
 */
bool Get::handleIndexFile()
{
	LOG(DEBUG, "handleIndexFile");
	if (setIndexFile(_path))
		return false;

	std::string indexHtml = _path + "/index.html";
	if ((_fd = open(indexHtml.c_str(), O_RDONLY)) >= 0)
		return false;

	if (_config->getAutoIndex())
	{
		std::vector<Directory> currDir = handleAutoindex(_path);
		_file = autoIndexToJson(currDir);
		_autoindex = true;
		return true;
	}
	throw Tools::Exception(403, "GET: cannot read a directory");
	return false;
}

/**
 * @brief Handles CGI directory listing requests.
 * If the query string contains 'format=json', it generates a JSON list of 
 * directory contents (scripts) and sets the autoindex state. Otherwise, 
 * it defaults to finding a standard index file.
 * @return true if JSON was generated; false if falling back to index file.
 */
bool Get::handleCgiPage() {
	std::map<std::string, std::string>::const_iterator it = _request.getQueryParams().find("format");
	if (it != _request.getQueryParams().end() && it->second == "json") {
		std::vector<Directory> currDir = handleAutoindex(_path);
		_file = autoIndexToJson(currDir);
		_autoindex = true;
		return true;
	}
	setIndexFile(_path);
	return false;
}

/**
 * @brief Checks whether the given path is a CGI location
 */
bool Get::isCgiLocation(const std::string& path) {
	const std::string& folderPath = _config->getCGIPaths()._scriptFolderPath;
	if (!folderPath.empty() && (path == folderPath || (Tools::getLastCharacter(path) == '/' && path.substr(0, path.size() - 1) == folderPath))) {
		if (!_config->hasCGI())
			throw Tools::Exception(403, "CGI usage is forbidden on this specific server");
		else
			return true;
	}
	return false;
}

void Get::checkAndSetFile(const std::string &path)
{
	_path = _config->getRoot() + path;
	if (Tools::isDirectory(_path.c_str()))
	{
		if (Tools::getLastCharacter(path) != '/')
			throw Tools::Exception(301, path + "/");
		if (isCgiLocation(path)) {
			if (handleCgiPage())
				return;
		}
		else if (handleIndexFile())
			return;
	}
	LOG(INFO, YELLOW_BRIGHT, "file path = " + _path);
	if (_fd == -1)
		_fd = openFile(_path);

	char buffer[BUFFERSIZE];
	ssize_t bytesRead = 0;

	while ((bytesRead = read(_fd, buffer, BUFFERSIZE)) > 0)
		_file.append(buffer, bytesRead);

	Tools::closeAndResetFD(_fd);

	if (bytesRead < 0)
	{
		if (errno == EISDIR)
			throw Tools::Exception(403, "GET: cannot read a directory");
		else
			throw Tools::Exception(500, "GET: read error");
	}
	LOG(INFO, "File = " + path);
}

const std::string& Get::prepareResponse(HttpResponse& response) {
	response.addDateHeader();

	response.setBody(_file);

	response.addHeader("Content-length", Tools::intToString(_file.size()));
	std::string extension;
	if (_autoindex)
		extension = Tools::getExtension("file.json");
	else
		extension = Tools::getExtension(getPath());
	if (!extension.empty())
	{
		if (extension == ".ico")
			extension = ".png"; // Hardcoded fix for icons
		else if (extension == ".jpeg")
			extension = ".jpg";
		response.addHeader("Content-Type", extension);
	}
	return response.getFinalResponse();
}

const std::string Get::executeGet(const HttpRequest &request, const ConfigBase *config)
{
	Get get(request, config);
	get.checkRequest();
	get.checkAndSetFile(request.getPurePath());

	HttpResponse response(HttpTools::getReturnPair(200));
	return get.prepareResponse(response);
}