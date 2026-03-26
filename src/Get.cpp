#include "Get.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Get::Get(const HttpRequest &request, const ConfigBase *config) : _request(request), _config(config) {}

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

void Get::checkAndSetFile(const std::string &path)
{
	std::clog << YELLOW_BRIGHT << "checkAndSetFile" << RESET << std::endl;
	std::string fullPath = _config->getRoot() + path;
	std::clog << YELLOW_BRIGHT << "fullpath = " << fullPath << RESET << std::endl;
	int fd = open(fullPath.c_str(), O_RDONLY);
	if (fd < 0)
	{
		if (errno == ENOENT)
			throw Tools::Exception(404, "GET: file not found");
		else if (errno == EACCES)
			throw Tools::Exception(403, "GET: not permitted");
		else
			throw Tools::Exception(500, "GET: unknown error, cannot open the file");
	}
	std::clog << YELLOW_BRIGHT << "Before reading file"<< RESET << std::endl;
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
	return response.getFinalResponse();
}