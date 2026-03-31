#include "Get.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Get::Get(const HttpRequest &request, const ConfigBase *config) : _fd(-1), _request(request), _config(config) {}

Get::~Get() { closeAndResetFD(); }

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

void Get::closeAndResetFD()
{
	if (_fd >= 0)
	{
		close(_fd);
		_fd = -1;
	}
}

bool Get::setIndexFile(const std::string &path)
{
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

const std::vector<Directory> Get::handleAutoindex(const std::string &path) const
{

}

const std::string Get::autoIndexToHTML(const std::vector<Directory> &currDir) const
{

}

/**
 * @brief First check if the config file has an index for the given path, then check if a index.html exists in the given path, then check and handle autoindex, and if none of the previous, throws an error.
 * @return false if the _fd is set and thus need to be open following the normal logic.
 * True if the _file is already being handled by the function and it doesn't have to follow the normal logic. 
 */
bool Get::handleIndexFile()
{
	if (setIndexFile(_path))
		return false;
	
	std::string indexHtml = _fd + "/index.html";
	if (_fd = open(indexHtml.c_str(), O_RDONLY) >= 0)
		return false;
	
	if (_config->getAutoIndex())
	{
		std::vector<Directory> currDir = handleAutoindex(_path);
		_file = autoIndexToHTML(currDir);
		return true;
	}
	throw Tools::Exception(403, "GET: cannot read a directory");
	return false;	
}

void Get::checkAndSetFile(const std::string &path)
{
	_path = _config->getRoot() + path;
	LOG(DEBUG, "ROOT = " + _config->getRoot());

	if (Tools::isDirectory(_path.c_str()))
	{
		if (handleIndexFile())
			return ;
	}
	LOG(INFO, YELLOW_BRIGHT, "file path = " + _path);
	if (_fd == -1)
		_fd = openFile(_path);
	
	char buffer[BUFFERSIZE];
	ssize_t bytesRead = 0;

	while ((bytesRead = read(_fd, buffer, BUFFERSIZE)) > 0)
		_file.append(buffer, bytesRead);
	
	closeAndResetFD();

	if (bytesRead < 0)
	{
		if (errno == EISDIR)
			throw Tools::Exception(403, "GET: cannot read a directory");
		else
			throw Tools::Exception(500, "GET: read error");
	}
	LOG(DEBUG, "File = " + path);
}

const std::string Get::getExtension() const
{
	std::string::size_type pos = _path.rfind(".");

	if (pos == std::string::npos || pos == 0)
    	return "";
	
	return HttpTools::getContentType(_path.substr(pos));
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
	std::string extension = get.getExtension();
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