
#include "ServerManager.hpp"
#include "HttpMethod.hpp"
#include "CGI.hpp"

std::vector<Server *> setupServers(const std::vector<ServerBlockConfig> &serverConfigs);

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ServerManager::~ServerManager()
{
	LOG(INFO, RED_BRIGHT, "Calling ServerManager's destructor");
	for (std::vector<ServerSocket *>::iterator it = _serverSocketArray.begin(); it != _serverSocketArray.end(); it++)
		delete (*it);
	for (std::vector<Server *>::iterator it = _serverArray.begin(); it != _serverArray.end(); it++)
		delete (*it);
	if (_polling)
		delete _polling;
}

ServerManager::ServerManager(const std::vector<ServerBlockConfig> &serverConfigs) : _polling(NULL)
{
	try
	{
		setupServers(serverConfigs);
		_serversMap = setupServersMap();
		_servSockFDs = setupServSockFDs();
		_polling = new Polling(_servSockFDs);
		LOG(INFO, "All servers set up successfully");
	}
	catch (Tools::Exception &e)
	{
		for (std::vector<ServerSocket *>::iterator it = _serverSocketArray.begin(); it != _serverSocketArray.end(); it++)
			delete (*it);
		for (std::vector<Server *>::iterator it = _serverArray.begin(); it != _serverArray.end(); it++)
			delete (*it);
		if (_polling)
			delete _polling;
		throw;
	}
}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
ServerManager &ServerManager::operator=(const ServerManager &obj)
{
	(void)obj;
	return (*this);
}

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

std::map<std::pair<int, std::string>, Server *> ServerManager::setupServersMap()
{
	std::map<std::pair<int, std::string>, Server *> tmpMap;
	std::vector<Server *>::iterator it = _serverArray.begin();
	for (; it != _serverArray.end(); it++)
	{
		std::set<std::string>::const_iterator sit = (*it)->getServerNames().begin();
		for (; sit != (*it)->getServerNames().end(); sit++)
			tmpMap.insert(std::make_pair(std::make_pair(std::atoi((*it)->getPort().c_str()), *sit), (*it)));
	}
	return tmpMap;
}

void ServerManager::setupServers(const std::vector<ServerBlockConfig> &serverConfigs)
{
	bool found = false;
	for (std::vector<ServerBlockConfig>::const_iterator mit = serverConfigs.begin(); mit != serverConfigs.end(); mit++)
	{
		std::vector<ServerSocket *>::iterator it = _serverSocketArray.begin();
		for (; it != _serverSocketArray.end(); it++)
		{
			if ((*it)->getPort() == mit->getPort())
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			// Create the socket here
			_serverSocketArray.push_back(new ServerSocket(mit->getIp(), mit->getPort()));
			_serverArray.push_back(new Server(*mit, _serverSocketArray.back()));
		}
		else
		{
			LOG(INFO, LIME, "ALREADY EXISTING SOCKET");
			_serverArray.push_back(new Server(*mit, *it));
		}
		found = false;
	}
}

std::set<int> ServerManager::setupServSockFDs()
{
	std::set<int> tempServSockFDs;
	std::vector<Server *>::const_iterator it = _serverArray.begin();
	for (; it != _serverArray.end(); it++)
	{
		LOG(INFO, YELLOW_BRIGHT, "setupServSockFDs for fd", Tools::intToString((*it)->getServSockFD()));
		tempServSockFDs.insert((*it)->getServSockFD());
	}

	return tempServSockFDs;
}

/**
 * @brief Find and return the correct port based on the client's socket FD
 */
const std::string ServerManager::findPort(Client *client)
{
	return Tools::intToString(client->getConnectedPort());
}

/**
 * @brief Build an object `std::pair<std::string, std::string>` based on `Server Name` and `Port`
 */
std::pair<std::string, std::string> buildHostPair(const std::string &str, const std::string &port)
{
	std::vector<std::string> split = Tools::splitString(str, ":");
	if (split[0] == "127.0.0.1")
		split[0] = "localhost";
	if (split.size() == 1)
		return std::make_pair(split[0], port);

	if (!Tools::isValidPort(split[1]))
		throw Tools::Exception(400, "Port of Host header is invalid");

	return std::make_pair(split[0], split[1]);
}

/**
 * @brief Find the correct `Server` object based on request's `Server Name` and `Port`
 */
Server *ServerManager::findServer(const std::string &host, const std::string &port, HttpRequest &request)
{
	std::pair<std::string, std::string> hostPair = buildHostPair(host, port);

	int targetPort = std::atoi(hostPair.second.c_str());
	std::pair<int, std::string> exactKey(targetPort, hostPair.first);

	request.setPort(port);
	std::map<std::pair<int, std::string>, Server *>::const_iterator it = _serversMap.find(exactKey);
	if (it != _serversMap.end())
	{ // found exact match for Port + Server Name
		LOG(INFO, LIGHT_BLUE, "Found exact match for ", Tools::intToString(exactKey.first) + ":" + exactKey.second);
		request.setServerName(it->first.second);
		return it->second;
	}

	std::pair<int, std::string> defaultKey(targetPort, "");
	it = _serversMap.lower_bound(defaultKey); // Find the first match for targetPort regardless of the Server Name
	if (it != _serversMap.end())
	{
		request.setServerName(it->first.second);
		LOG(INFO, LIGHT_BLUE, "Found match for default port " + Tools::intToString(it->first.first) + " with server name: " + it->first.second);
		return it->second;
	}

	throw Tools::Exception(500, "Error finding server");
}

/**
 * @brief Find the correct `Server` object based on request's `Server Name` and `Port`, used in findMaxBodySize()
 */
Server *ServerManager::findServer(const std::string &host, const std::string &port)
{
	std::pair<std::string, std::string> hostPair = buildHostPair(host, port);

	int targetPort = std::atoi(hostPair.second.c_str());
	std::pair<int, std::string> exactKey(targetPort, hostPair.first);

	std::map<std::pair<int, std::string>, Server *>::const_iterator it = _serversMap.find(exactKey);
	if (it != _serversMap.end()) // found exact match for Port + Server Name
		return it->second;

	std::pair<int, std::string> defaultKey(targetPort, "");
	it = _serversMap.lower_bound(defaultKey); // Find the first match for targetPort regardless of the Server Name
	if (it != _serversMap.end())
		return it->second;

	throw Tools::Exception(500, "Error finding server");
}

/**
 * @brief Find the matching `ConfigBase` object based on the client's request and return it
 */
const ConfigBase *ServerManager::findConfigBase(Client &client, HttpRequest &request)
{
	std::string port = findPort(&client);
	(void)client;
	std::map<std::string, std::string>::const_iterator it = request.getHeader().find("host");
	if (it == request.getHeader().end())
	{
		client.setToBeClosed(true);
		throw Tools::Exception(400, "Host header missing");
	}
	Server *server = findServer(it->second, port, request);
	std::string modifiableString(request.getPurePath());
	return &server->getPathConfig(modifiableString);
}

long ServerManager::findMaxBodySize(Client *client, const std::string &host, std::string path)
{
	std::string port = findPort(client);

	Server *server = findServer(host, port);
	std::string modifiableString(path);
	return server->getPathConfig(modifiableString).getClientMaxBodySize();
}

/**
 * @brief Check if request's path has a return directive or a method that isn't allowed/implemented
 */
void handleReturnAndAllowMethod(const ConfigBase *config, const std::string &method)
{
	if (config->getReturnDirective().first)
		throw Tools::Exception(config->getReturnDirective().first, config->getReturnDirective().second);

	std::set<std::string>::const_iterator it = config->getAllowMethods().find(method);
	if (it == config->getAllowMethods().end())
		throw Tools::Exception(405, "Method not allowed");

	if (method != "GET" && method != "POST" && method != "DELETE")
		throw Tools::Exception(501, "Method " + method + " not implemented");
}

void ServerManager::sendResponse(Client *client)
{
	if (!client)
		throw Tools::Exception("sendResponse: no client, undefined behavior");
	else if (client->getBytesSent() > client->getResponseBuff().size())
		throw Tools::Exception("sendResponse: incorrect response size");
	int sent = send(client->getFD(), client->getResponseBuff().c_str() + client->getBytesSent(), client->getResponseBuff().size() - client->getBytesSent(), MSG_NOSIGNAL);
	LOG(INFO, YELLOW_BRIGHT, "BYTES SENT", Tools::intToString(sent));

	if (sent < 0)
	{
		client->setToBeClosed(true);
		throw Tools::Exception("sendResponse = -1");
	}
	client->addBytesSent(sent);
	if (client->getBytesSent() >= client->getResponseBuff().size())
		client->setResponseSent(true);
}

void checkBodySize(std::size_t size, std::size_t max)
{
	if (size > max)
		throw Tools::Exception(413, "body above max body size");
}

bool isValidCgiPath(const HttpRequest &request, const ConfigBase *config)
{
	const std::string extension = Tools::extractExtension(request.getPurePath());
	if (extension == ".py" || extension == ".php" || extension == ".sh")
	{
		if (!config->hasCGI())
			throw Tools::Exception(403, "CGI usage is forbidden on this specific server");
		else
			return true;
	}
	return false;
}

/**
 * @brief execute the HTTP method or the CGI and return the formatted HTTP response.
 * @return the formatted HTTP response in case of success
 * @throw in case of error
 */
const std::string ServerManager::execute(const HttpRequest &request, const ConfigBase *config, Client *client) // client *
{
	LOG(INFO, YELLOW_BRIGHT, "execute");
	std::string response;

	if (isValidCgiPath(request, config))
	{
		LOG(INFO, "Initiating CGI");
		CGI *cgi = new CGI(request, config);
		_polling->getCgiMap()[cgi] = client;
		_polling->addFdToEpoll(cgi->getPipeOut(), EPOLLIN);
		if (cgi->getPostPipeIn() >= 0)
		{
			_polling->addFdToEpoll(cgi->getPostPipeIn(), EPOLLOUT);
		}
		return "";
	}

	else if (request.getMethod() == "GET")
	{
		checkBodySize(request.getBody().size(), static_cast<std::size_t>(config->getClientMaxBodySize()));
		response = Get::executeGet(request, config);
	}

	else if (request.getMethod() == "POST")
	{
		checkBodySize(request.getBody().size(), static_cast<std::size_t>(config->getClientMaxBodySize()));
		response = Post::executePost(request, config);
	}

	else if (request.getMethod() == "DELETE")
	{
		response = Delete::executeDelete(request, config);
	}

	return response;
}

void ServerManager::setResponseAndDeleteCGI(int eventFD, const std::pair<CGI *, Client *> &it)
{
	// Close pipe which also removes it from epoll
	Tools::closeAndResetFD(eventFD);
	// Parse the CGI output and get the headers
	try
	{
		it.second->setResponseBuff(it.first->getResponse());
	}
	catch (Tools::Exception &e)
	{
		_polling->getCgiMap().erase(it.first);
		const ConfigBase *config = it.first->getConfig();
		delete it.first;
		throwHandler(it.second, e, config, true);
		return ;
	}
	it.second->setDoneReceiving(true);
	it.second->setResponseToBeSent(true);
	handleResponse(it.second);
	_polling->getCgiMap().erase(it.first);
	delete it.first;
}

/**
 * @brief If using the Config file error page and opening the ERROR_PAGE_TEMPLATE_PATH both fail for whatever reason,
 * this function generates a hardcoded, very minimal error page.
 * @returns Minimal HTML code to be used as a body in the HTTP response
 * @attention Replace the template strings {(CODE)} and {(MSG)} with the HTTP code and message
 */
const std::string generateFallBackPage()
{
	return "<html><body><h1>{(CODE)} {(MSG)}</h1></body></html>";
}

const std::string generateErrorPage(int code);

/**
 * @brief Read error file into a string and return it
 * @param errorPath Path of the error file
 * @param code HTML error code
 * @param isPredefined Whether `code` has a predefined error file path mentioned in the config file
 * @attention May return a file with template `{(CODE)}` and `{(MESSAGE)}` strings that should to be replaced if `isPredefined` is false
 */
const std::string readErrorFile(const std::string &errorPath, int code, bool isPredefined)
{
	int fd = open(errorPath.c_str(), O_RDONLY);
	if (fd == -1)
	{
		// If the error page defined in the config file fails while trying to open it,
		// generate a templated error page by the server
		if (isPredefined)
			return generateErrorPage(code);

		// If opening the server defined template file fails, generate a hard coded, minimal
		else
			return generateFallBackPage();
	}
	std::string finalString;
	char buffer[BUFFERSIZE];
	ssize_t bytesRead = 0;
	while ((bytesRead = read(fd, buffer, BUFFERSIZE)) > 0)
		finalString.append(buffer, bytesRead);
	Tools::closeAndResetFD(fd);
	if (bytesRead == -1)
		return "";
	return finalString;
}

/**
 * @brief Generate and return HTML error file body
 */
const std::string generateErrorPage(int code)
{
	const char *workingDirectory = std::getenv("PWD");
	std::string errorPagePath = ERROR_PAGE_TEMPLATE_PATH;
	std::string fullPath(std::string(workingDirectory) + errorPagePath);
	std::string fileContent = readErrorFile(fullPath, code, false);
	Tools::findAndReplaceAllOccurrences(fileContent, TEMPLATE_ERROR_CODE, Tools::intToString(code));
	Tools::findAndReplaceAllOccurrences(fileContent, TEMPLATE_ERROR_MESSAGE, HttpTools::getHttpReturnMessage(code));
	return fileContent;
}

/**
 * @brief Creates and returns a HTML responses for the redirect HTML codes `300`-`308`
 */
const std::string handleRedirect(Tools::Exception &e)
{
	HttpResponse response(HttpTools::getReturnPair(e.getReturnCode()));
	const std::string &location = e.getMsgLog();
	const int code = e.getReturnCode();
	if (code == 301 || code == 302 || code == 303 || code == 305 || code == 307 || code == 308)
		response.addHeader("Location", location);

	if (code == 300)
	{
		response.setBody("<a href=\"/v1\">Ver 1</a><br><a href=\"/v2\">Ver 2</a>");
		response.addHeader(CONTENT_LENGTH, Tools::intToString(response.getBody().size()));
	}
	else
		response.addHeader(CONTENT_LENGTH, "0");
	response.addDateHeader();

	return response.getFinalResponse();
}

/**
 * @brief Creates and returns a HTML "error" response for HTTP codes that aren't redirect codes `300` - `308`
 */
const std::string handleOtherCodes(const ConfigBase *config, int httpCode)
{
	if (httpCode == 0)
		httpCode = 500;
	std::string errorBody;
	if (config)
	{
		HttpTools::MapType::const_iterator it = config->getErrorPages().find(httpCode);
		if (it == config->getErrorPages().end())
			errorBody = generateErrorPage(httpCode);
		else
		{
			errorBody = readErrorFile(config->getRoot() + it->second, httpCode, true);
		}
	}
	else
		errorBody = generateErrorPage(httpCode);

	HttpResponse response(HttpTools::getReturnPair(httpCode));

	// Manually set return code and message if the code is a custom one
	if (HttpTools::getReturnPair(httpCode).second == "")
	{
		response.setReturnCode(httpCode);
		response.setReturnMessage("Custom Error");
	}
	response.setBody(errorBody);
	response.addHeader(CONTENT_LENGTH, Tools::intToString(errorBody.size()));
	response.addHeader(CONTENT_TYPE, HttpTools::getContentType(".html"));
	LOG(INFO, LIGHT_BLUE, "Error file size is", Tools::intToString(errorBody.size()));
	response.addDateHeader();

	return response.getFinalResponse();
}

/**
 * @brief HTML error code handler
 */
void ServerManager::throwHandler(Client *client, Tools::Exception &e, const ConfigBase *config, bool reThrow)
{
	if (!client)
	{
		if (reThrow)
			throw;
		return;
	}
	if (e.getReturnCode() >= 400)
		LOG(ERROR, e.getMsgLog());
	else
		LOG(DEBUG, PINK, "Throw code " + Tools::intToString(e.getReturnCode()), e.getMsgLog());

	if (e.getReturnCode() >= 100 || e.getReturnCode() == 0)
	{
		std::string responseString;
		if (e.getReturnCode() >= 300 && e.getReturnCode() <= 308)
			responseString = handleRedirect(e);
		else
			responseString = handleOtherCodes(config, e.getReturnCode());

		LOG(DEBUG, responseString);
		client->refreshClient();
		client->setResponseBuff(responseString);
		try
		{
			static const int excludingCodes[] = {408, 413, 429, 431, 500, 502, 503, 504};
			static const int excludingCodesSize = sizeof(excludingCodes) / sizeof(excludingCodes[0]);
			for (int i = 0; i < excludingCodesSize; ++i)
			{
				if (e.getReturnCode() == excludingCodes[i])
				{
					client->setToBeClosed(true);
				}
			}
			sendResponse(client);
		}
		catch (Tools::Exception &e)
		{
			if (client->toBeClosed())
			{
				if (!_polling->deleteClient(client))
					throw Tools::Exception("Error at deleting client");
				client = NULL;
			}
			LOG(CRITICAL, "Response THROWS in throwHandler");
		}
	}

	if (client && client->toBeClosed())
	{
		if (!_polling->deleteClient(client))
			throw Tools::Exception("Error at deleting client");
		client = NULL;
	}

	// ============================================================================
	// NOT SURE IF WE SHOULD REFRECH THE CLIENT HERE
	if (client)
	{
		_polling->setClientEPOLLOUT(client, false);
		client->refreshClient();
	}
	// ============================================================================
	if (reThrow)
		throw;
}

void ServerManager::handleResponse(Client *client)
{
	if (!client)
		return;
	if (client->doneReceiving())
	{
		if (client->responseToBeSent() && !client->readyToReceive())
		{
			// Set the EPOLLOUT event to be monitored.
			_polling->setClientEPOLLOUT(client, true);
			client->setReadyToReceive(true);
		}
		LOG(INFO, PURPLE, "toReceive", Tools::boolToString(client->readyToReceive()));
		LOG(INFO, PURPLE, "toBeSent", Tools::boolToString(client->responseToBeSent()));
		if (client->readyToReceive() && client->responseToBeSent())
		{
			sendResponse(client);
		}
		if (client->responseSent())
		{
			// Remove the EPOLLOUT event
			_polling->setClientEPOLLOUT(client, false);
			client->refreshClient();
		}
	}
}

std::string ServerManager::requestPreParsing(Client *client)
{
	client->bufferManager();
	if (client->getHost().empty() || client->getPath().empty())
	{
		return "";
	}
	client->setMaxBodySize(findMaxBodySize(client, client->getHost(), client->getPath()));
	std::string tmpRequest = client->bodyVerification();
	return tmpRequest;
}

void handleKeepAlive(Client *client, const HttpRequest &request)
{
	std::string keepAlive = request.findHeader("connection");
	if (keepAlive.empty() || keepAlive.compare("close"))
		return;
	client->setKeepAlive(false);
}

void ServerManager::clientLogic(Client *client)
{
	const ConfigBase *config = NULL;
	LOG(DEBUG, BLUE_BRIGHT, "clientLogic");
	try
	{
		client->updateTimestamp();
		std::string tmpRequest = requestPreParsing(client);
		if (client->doneReceiving())
		{
			LOG(DEBUG, BLUE_BRIGHT, "CLIENT IS DONE RECEIVING");
			HttpRequest request(client->getClientAddr());
			request.parse(tmpRequest);
			handleKeepAlive(client, request);

			// Ideally we would call this function after the headers are parsed, for now it is here
			config = findConfigBase(*client, request);
			handleReturnAndAllowMethod(config, request.getMethod());

			client->setResponseBuff(execute(request, config, client)); // on envoie le client
			if (!client->getResponseBuff().empty())
				client->setResponseToBeSent(true);
			handleResponse(client);
		}
		// client->printStatus();
		if (client->toBeClosed())
		{
			_polling->deleteClient(client);
			return;
		}
	}
	catch (Tools::Exception &e)
	{
		if (e.getReturnCode() == 42)
		{
			throw;
		}
		throwHandler(client, e, config, true);
	}
	catch (std::exception &e)
	{
		Tools::Exception error(500, e.what());
		throwHandler(client, error, config, true);
	}
}

bool ServerManager::matchServerFD(int eventFD) const
{
	if (_servSockFDs.find(eventFD) != _servSockFDs.end())
	{
		LOG(INFO, ORANGE, "matchServerFD new client found from FD", Tools::intToString(eventFD));
		return true;
	}
	return false;
}

void ServerManager::cgiTimeout()
{
	pid_t pid = 0;
	int status = 0;

	std::map<CGI *, Client *> cgis = _polling->getCgiMap();

	for (std::map<CGI *, Client *>::iterator it = cgis.begin(); it != cgis.end(); it++ /* no increment here *insert favicon.ico* */)
	{
		if (it->first->getHasFinishedExecuting())
			continue;
		pid = waitpid(it->first->getPid(), &status, WNOHANG);

		if (pid == 0)
		{
			if (std::time(NULL) - it->first->getTimeStamp() > CGI_TIMEOUT)
			{
				kill(it->first->getPid(), SIGKILL);
				it->second->setToBeClosed(true);

				Tools::Exception e(504, "cgiTimeout: Script took too long to execute");
				throwHandler(it->second, e, NULL, false);
				continue;
			}
			// ++it; // Move to next if not timed out
		}
		else if (pid == -1)
		{
			it->second->setToBeClosed(true);
			Tools::Exception e(502, "cgiTimeout: waitpid returned -1, CGI has somehow failed");
			throwHandler(it->second, e, NULL, false);
		}
		else if (pid > 0)
		{
			it->first->setHasFinishedExecuting(true);
			if (WIFEXITED(status))
				LOG(INFO, SKY_BLUE, "CGI exited");
			if (WEXITSTATUS(status) > 0) {
				LOG(INFO, SKY_BLUE, "CGI exit status is " + Tools::intToString(WEXITSTATUS(status)));
				it->second->setToBeClosed(true);

				Tools::Exception e(500, "cgiTimeout: Script failed to execute");
				throwHandler(it->second, e, NULL, false); 
				continue;
			}
			else if (WIFSIGNALED(status))
				LOG(INFO, RED, "CGI script was killed");
		}
	}
}

void ServerManager::handleTimeout()
{
	const std::time_t currTime = std::time(0);
	std::vector<Client *> &clients = _polling->getClientVector();

	for (std::vector<Client *>::reverse_iterator it = clients.rbegin();
		 it != clients.rend();)
	{
		Client *client = *it;
		if (!client)
		{
			++it;
			continue;
		}
		if ((currTime - client->getTimestamp()) > TIMEOUT)
		{
			client->setToBeClosed(true);
			std::string message = "timeout for client fd = " + Tools::intToString(client->getFD());
			Tools::Exception timeoutException(408, message);
			
			// Call throwHandler without throwing to bypass the normal logic.
			throwHandler(client, timeoutException, NULL, false);
			it = clients.rbegin();
			continue;
		}
		++it;
	}
}

void ServerManager::router(const epoll_event &event)
{
	Client *client = NULL;
	LOG(DEBUG, PURPLE, "In router");

	int eventFD = event.data.fd;
	client = _polling->handleClientEvent(eventFD, event.events);
	if (client)
		clientLogic(client);
	else
	{
		LOG(DEBUG, BLUE_BRIGHT, "ELSE ROUTER");
		std::map<CGI *, Client *>::const_iterator it = _polling->getCgiMap().begin();
		for (; it != _polling->getCgiMap().end(); ++it)
		{
			// It's a CGI
			if (it->first->getPipeOut() == event.data.fd)
			{
				if (it->first->readCgiOutput())
					setResponseAndDeleteCGI(event.data.fd, *it);
				LOG(DEBUG, "PIPEOUT");
				return;
			}
			// It's the POST write end of the CGI
			else if (it->first->getPostPipeIn() == event.data.fd)
			{
				it->first->handlePostCGI();
				LOG(DEBUG, "POSTPIPE");
				return;
			}
			LOG(DEBUG, "CGI ELSE");
		}
		LOG(INFO, "router: GHOST FD FOUND");
		Tools::closeAndResetFD(eventFD);
	}
}

void ServerManager::eventLoop()
{
	while (!_sigStop)
	{
		_polling->epollWaitEvent();
		if (_polling->getEventCount() == -1)
		{
			if (errno == EINTR)
				return;
		}

		// tentative de gestion des timeout (c'est Charles qui code source tkt)
		handleTimeout();
		cgiTimeout();

		const epoll_event *eventArray = _polling->getEventArray();
		for (int i = 0; i < _polling->getEventCount(); i++)
		{
			int eventFD = eventArray[i].data.fd;
			_polling->setCurrEventFD(eventFD);
			if (matchServerFD(eventFD))
				_polling->registerNewClient(eventFD);
			else
				router(eventArray[i]);
		}
	}
}

void ServerManager::mainLoop()
{
	while (!_sigStop)
	{
		try
		{
			eventLoop();
		}
		catch (Tools::Exception &e)
		{
			if (e.getReturnCode() == 42)
				throw;
			LOG(ERROR, RED_BRIGHT, "mainLoop", e.getMsgLog());
		}
		catch (std::exception &e)
		{
			LOG(CRITICAL, e.what());
		}
		catch (...)
		{
			LOG(CRITICAL, "Undefined error");
		}
	}
}
