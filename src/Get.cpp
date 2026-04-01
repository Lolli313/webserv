#include "Get.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Get::Get(const HttpRequest &request, const ConfigBase *config) : _fd(-1), _request(request), _config(config), _autoindex(false) {}

Get::~Get() { Tools::closeAndResetFD(_fd); }

// Get::Get(const Get &obj) { *this = obj; }

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
// Get &Get::operator=(const Get &obj)
// {
// 	(void)obj;
// 	return (*this);
// }

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
 * @brief First check if the config file has an index for the given path, then check if a index.html exists in the given path, then check and handle autoindex, and if none of the previous, throws an error.
 * @return false if the _fd is set and thus need to be open following the normal logic.
 * True if the _file is already being handled by the function and it doesn't have to follow the normal logic.
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

void Get::checkAndSetFile(const std::string &path)
{
	LOG(DEBUG, "checkAndSetFile");
	_path = _config->getRoot() + path;
	LOG(DEBUG, "ROOT = " + _config->getRoot());
	LOG(DEBUG, "PATH = " + _path);

	if (Tools::isDirectory(_path.c_str()))
	{
		if (handleIndexFile())
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
	LOG(DEBUG, "File = " + path);
}

const std::string Get::getExtension(const std::string &path) const
{
	std::string::size_type pos = path.rfind(".");

	if (pos == std::string::npos || pos == 0)
		return "";

	return HttpTools::getContentType(path.substr(pos));
}

const std::string Get::executeGet(const HttpRequest &request, const ConfigBase *config)
{
	LOG(INFO, YELLOW_BRIGHT, "executeGET");
	Get get(request, config);
	get.checkRequest();
	LOG(DEBUG, "PUREPATH = " + request.getPurePath());
	get.checkAndSetFile(request.getPurePath());

	HttpResponse response(HttpTools::getReturnPair(200));
	response.addDateHeader();

	response.setBody(get._file);

	response.addHeader("Content-length", Tools::intToString(get._file.size()));
	std::string extension;
	if (get._autoindex)
		extension = get.getExtension("file.json");
	else
		extension = get.getExtension(get.getPath());
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