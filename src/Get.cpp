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
	std::map<std::string, std::string>::const_iterator it = _request.getHeader().find("Host");
	if (it == _request.getHeader().end())
		throw Tools::Exception(400, "GET: host not found.");
	_host = it->second;
}

void Get::checkAndSetFile(const std::string &path)
{
	std::string fullPath = _config->getRoot() + path;
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
	char buffer[BUFFERSIZE];
	ssize_t bytesRead = 0;
	while ((bytesRead = read(fd, buffer, BUFFERSIZE)) > 0)
		_file.append(buffer, bytesRead);
	if (bytesRead < 0)
			throw Tools::Exception(500, "GET: read error");
}


const std::string Get::executeGet(const HttpRequest &request, const ConfigBase *config)
{
	Get get(request, config);
	get.checkRequest();
	get.checkAndSetFile(request.getPurePath());

	HttpResponse response(HttpTools::getReturnPair(200));
	response.addDateHeader();
	response.setBody(get._file);
	response.addHeader("Content-length", Tools::intToString(get._file.size()));
	return response.getFinalResponse();
}