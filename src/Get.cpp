#include "Get.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Get::Get(const HttpRequest &request, const ConfigBase *config) : _request(request), _config(config), _rootDir(ROOT_DIR) {}

Get::~Get() {}

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
	std::clog << YELLOW_BRIGHT << "checkRequest" << RESET << std::endl;
	std::map<std::string, std::string>::const_iterator it = _request.getHeader().find("Host");
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

void Get::setIndexFile()
{
	for (std::vector<std::string>::const_iterator it = _config->getIndex().begin(); it != _config->getIndex().end(); it++)
	{
		std::string currIndex = _path + "/" + *it;
		if (Tools::fileExists(currIndex.c_str()))
		{
			_path.append(*it);
			return ;
		}
	}
}

void Get::checkAndSetFile(const std::string &path)
{
	_path = _config->getRoot() + path;
	if (_path.size() >= _rootDir.size() &&
    _path.compare(_path.size() - _rootDir.size(), _rootDir.size(), _rootDir) == 0)
		setIndexFile();
	std::clog << YELLOW_BRIGHT << "file path = " << _path << RESET << std::endl;
	int fd = openFile(_path);
	char buffer[BUFFERSIZE];
	ssize_t bytesRead = 0;
	while ((bytesRead = read(fd, buffer, BUFFERSIZE)) > 0)
		_file.append(buffer, bytesRead);
	close(fd);
	if (bytesRead < 0)
	{
		if (errno == EISDIR)
			throw Tools::Exception(404, "GET: cannot read a directory");
		else
			throw Tools::Exception(500, "GET: read error");
	}
	std::clog << YELLOW_BRIGHT << "File = " << _file << RESET << std::endl;
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
	std::clog << YELLOW_BRIGHT << "executeGET" << RESET << std::endl;
	Get get(request, config);
	get.checkRequest();
	get.checkAndSetFile(request.getPurePath());

	HttpResponse response(HttpTools::getReturnPair(200));
	response.addDateHeader();
	response.setBody(get._file);
	response.addHeader("Content-length", Tools::intToString(get._file.size()));
	std::string extension = get.getExtension();
	if (!extension.empty())
		response.addHeader("Content-Type", extension);
	return response.getFinalResponse();
}