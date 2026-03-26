
#include "ServerManager.hpp"
#include "HttpMethod.hpp"

std::vector<Server *> setupServers(const std::vector<ServerBlockConfig> &serverConfigs);

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ServerManager::~ServerManager()
{
	std::clog << RED << "Calling ServerManager's destructor" << RESET << std::endl;
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
			_serverSocketArray.push_back(new ServerSocket(mit->getPort()));
			_serverArray.push_back(new Server(*mit, _serverSocketArray.back()));
		}
		else
		{
			std::clog << "ALREADY EXISTING SOCKET" << std::endl;
			_serverArray.push_back(new Server(*mit, *it));
		}
		found = false;
		// std::clog << CYAN_BRIGHT << "setupServers for fd = " << mit->getServSockFD() << RESET << std::endl;
	}
}

std::set<int> ServerManager::setupServSockFDs()
{
	std::set<int> tempServSockFDs;
	std::vector<Server *>::const_iterator it = _serverArray.begin();
	for (; it != _serverArray.end(); it++)
	{
		std::clog << YELLOW_BRIGHT << "setupServSockFDs for fd = " << (*it)->getServSockFD() << RESET << std::endl;
		tempServSockFDs.insert((*it)->getServSockFD());
	}

	return tempServSockFDs;
}

// Just a test response that directly sends to the client.
void TEST_RESPONSE(Client *tmpClient, int code, const std::string &message, const std::string &path)
{
	(void)message;
	(void)path;
	std::clog << "SENT" << std::endl;
	HttpResponse response(HttpTools::getReturnPair(code));
	// std::ifstream file(path.c_str());
	// std::ostringstream body;
	// body << file.rdbuf();
	// //response.setBody(body.str());
	// std::vector<std::pair<std::string, std::string> > tmp;
	// tmp.push_back(std::make_pair<std::string, std::string>("Content-Length", Tools::intToString(body.str().size())));
	// response.setResponseHeaders(tmp);
	response.addDateHeader();
	response.addHeader("Content-length", "0");
	send(tmpClient->getFD(), response.getFinalResponse().c_str(), response.getFinalResponse().size(), MSG_NOSIGNAL);
}

const std::string &ServerManager::findPort(int eventFD)
{
	std::vector<ServerSocket *>::const_iterator it = _serverSocketArray.begin();
	for (; it != _serverSocketArray.end(); it++)
	{
		if (eventFD == (*it)->getServSockFD())
			return (*it)->getPort();
	}
	return _serverSocketArray[0]->getPort();
}

std::pair<std::string, std::string> splitHostPair(const std::string &str)
{
	std::vector<std::string> split = Tools::splitString(str, ":");
	return std::make_pair(split[0], split[1]);
}

Server *ServerManager::findServer(const std::string &host)
{
	std::pair<std::string, std::string> hostPair = splitHostPair(host);

	int targetPort = std::atoi(hostPair.second.c_str());
	std::pair<int, std::string> exactKey(targetPort, hostPair.first);

	std::map<std::pair<int, std::string>, Server *>::const_iterator it = _serversMap.find(exactKey);
	if (it != _serversMap.end())
	{ // found exact match for Port + Server Name
		std::clog << LIGHT_BLUE << "Found exact match for " << exactKey.first << ":" << exactKey.second << RESET << std::endl;
		return it->second;
	}

	std::pair<int, std::string> defaultKey(targetPort, "");
	it = _serversMap.lower_bound(defaultKey); // Find the first match for targetPort regardless of the Server Name
	if (it != _serversMap.end())
	{
		std::clog << LIGHT_BLUE << "Found match for default port " << it->first.first << " with server name: " << it->first.second << RESET << std::endl;
		return it->second;
	}

	throw Tools::Exception(500, "Error finding server");
}

const ConfigBase *ServerManager::findConfigBase(const Client &client, const HttpRequest &request, int eventFD)
{
	std::string port = findPort(eventFD);
	(void)client;
	std::map<std::string, std::string>::const_iterator it = request.getHeader().find("Host");
	if (it == request.getHeader().end())
	{
		throw Tools::Exception(400, "Host header missing");
		std::clog << LIGHT_BLUE << "Host header missing" << RESET << std::endl;
	}
	Server *server = findServer(it->second);
	std::string modifiableString(request.getPath());
	return &server->getPathConfig(modifiableString);
}

void handleReturnAndAllowMethod(const ConfigBase *config, const std::string &method)
{
	if (config->getReturnDirective().first)
	{
		std::clog << LIGHT_BLUE << "Found return directive with code " << config->getReturnDirective().first << RESET << std::endl;
		throw Tools::Exception(config->getReturnDirective().first, config->getReturnDirective().second);
	}

	std::set<std::string>::const_iterator it = config->getAllowMethods().find(method);
	if (it == config->getAllowMethods().end())
	{
		throw Tools::Exception(405, "Method not allowed");
		std::clog << LIGHT_BLUE << method << " method not allowed" << RESET << std::endl;
	}
}

void ServerManager::sendResponse(Client *client)
{
	int sent = send(client->getFD(), client->getResponseBuff().c_str() + client->getBytesSent(), client->getResponseBuff().size() - client->getBytesSent(), MSG_NOSIGNAL);

	if (sent < 0)
		throw Tools::Exception("sendResponse = -1");
	client->addBytesSent(sent);
	if (client->getBytesSent() >= client->getBuffer().size())
	{
		client->setResponseSent(true);
	}
}

/**
 * @brief execute the HTTP method and return the formatted HTTP response.
 * @return the formatted HTTP response in case of success
 * @throw in case of error
 */
const std::string execute(const HttpRequest &request, const ConfigBase *config)
{
	std::clog << YELLOW_BRIGHT << "excecute" << RESET << std::endl;
	std::string response;
	if (request.getMethodStr() == "GET")
		return response = Get::executeGet(request, config);

	else if (request.getMethodStr() == "POST")
		return response = Post::executePost(request);

	else if (request.getMethodStr() == "DELETE")
	{
		return response = Delete::executeDelete(request, config);
	}
	return response;
}

// std::map<std::string, std::string>::const_iterator itCookie = request.getHeader().find("Cookie");
// if (itCookie != request.getHeader().end()) {
// 	_cookie.setCookie(itCookie->second);
// 	// _cookie.printCookie();
// }

/**
 * @brief If using the Config file error page and opening the ERROR_PAGE_TEMPLATE_PATH both fail for whatever reason,
 * this function generates a hardcoded, very minimal error page.
 * @returns Minimal HTML code to be used as a body in the HTTP response
 * @attention Replace the template strings {(CODE)} and {(MSG)} with the HTTP code and message
 */
const std::string generateFallBackPage() {
	return "<html><body><h1>{(CODE)} {(MSG)}</h1></body></html>";
}

const std::string generateErrorPage(int code);

const std::string readErrorFile(const std::string& errorPath, int code, bool isPredefined) {
	int fd = open(errorPath.c_str(), O_RDONLY);
	if (fd == -1) {
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
	close(fd);
	if (bytesRead == -1)
		return "";
	return finalString;
}

const std::string generateErrorPage(int code) {
	const char* workingDirectory = std::getenv("PWD");
	std::string errorPagePath = ERROR_PAGE_TEMPLATE_PATH;
	std::string fullPath(std::string(workingDirectory) + errorPagePath);
	std::string fileContent = readErrorFile(fullPath, code, false);
	Tools::findAndReplaceAllOccurences(fileContent, TEMPLATE_ERROR_CODE, Tools::intToString(code));
	Tools::findAndReplaceAllOccurences(fileContent, TEMPLATE_ERROR_MESSAGE, HttpTools::getHttpReturnMessage(code));
	return fileContent;
}

void ServerManager::throwHandler(Client *tmpClient, Tools::Exception &e, const ConfigBase *config)
{
	if (!tmpClient)
		throw;
	std::clog << PINK << "THROW ";
	std::clog << PINK << e.getMsgLog() << RESET << std::endl;
	if (e.getReturnCode() >= 100)
	{
		std::clog << LIGHT_BLUE << e.getMsgLog() << RESET << std::endl;
		// ===============================
		// HOW TO GET THE ERROR FILES ???
		// get value of server.getPathConfig() to a temp variable ConfigBase,
		// fix the code of that getPathConfig() function (bring the code from checkRequestValidity)
		// and pass the ConfigBase as parameter to the throwHandler()
		// and pass the ConfigBase as parameter to POST and GET method handlers
		// ===============================

		std::string errorBody;
		if (config) {
			HttpTools::MapType::const_iterator it = config->getErrorPages().find(e.getReturnCode());
			if (it == config->getErrorPages().end())
				errorBody = generateErrorPage(e.getReturnCode());
			else {
				errorBody = readErrorFile(config->getRoot() + it->second, e.getReturnCode(), true);
			}
		}
		else
			errorBody = generateErrorPage(e.getReturnCode());
		
		HttpResponse response(HttpTools::getReturnPair(e.getReturnCode()));
		response.setBody(errorBody);
		response.addHeader("Content-Length", Tools::intToString(errorBody.size()));
		std::clog << LIGHT_BLUE << "Error response body size is: " << errorBody.size() << RESET << std::endl;
		response.addDateHeader();

		tmpClient->setResponseBuff(response.getFinalResponse());
		sendResponse(tmpClient);
	}

	if (tmpClient->toBeClosed())
	{
		if (!_polling->deleteCLient(tmpClient))
			throw Tools::Exception("Error at deleting client");
		tmpClient = NULL;
	}

	// ============================================================================
	// NOT SURE IF WE SHOULD REFRECH THE CLIENT HERE
	if (tmpClient)
	{
		_polling->setClientEPOLLOUT(tmpClient, false);
		tmpClient->refreshClient();
	}
	// ============================================================================

	throw;
}

void ServerManager::existingClient(int eventFD)
{
	Client *tmpClient = _polling->handleExistingClient(eventFD, _polling->getEventArray()->events);
	const ConfigBase *config = NULL;
	if (tmpClient)
	{
		try
		{
			// TEST_RESPONSE(tmpClient, 404, "actually", "files/ascii/dog.html");			
			// std::clog << RED << tmpClient->getBuffer() << RESET << std::endl;
			std::string tmpRequest = tmpClient->bufferManager();
			// std::clog << GREEN << tmpClient->getBuffer() << RESET << std::endl;
			// std::string tmpRequest =
			// 	"GET /ascii/body.txt HTTP/1.1\r\n"
			// 	"Host: localhost:8080\r\n"
			// 	"\r\n";
			// tmpClient->setDoneReceiving(true);
			// tmpClient->setDoneReceiving(true);
			if (tmpClient->doneReceiving())
			{
				HttpRequest request;
				request.parse(tmpRequest);
				// Ideally we would call this function after the headers are parsed, for now it is here
				config = findConfigBase(*tmpClient, request, eventFD);
				handleReturnAndAllowMethod(config, request.getMethodStr());

				tmpClient->setResponseBuff(execute(request, config));
				tmpClient->setResponseToBeSent(true);
				// cookies
				// /uploads/images/img.png

				//	request.execute();

				if (tmpClient->responseToBeSent() && !tmpClient->readyToReceive())
				{
					// Set the EPOLLOUT event to be monitored.
					_polling->setClientEPOLLOUT(tmpClient, true);
					tmpClient->setReadyToReceive(true);
				}
				std::clog << RED << "toReceive : " << tmpClient->readyToReceive() << " toBeSent : " << tmpClient->responseToBeSent() << RESET << std::endl;
				if (tmpClient->readyToReceive() && tmpClient->responseToBeSent())
				{
					sendResponse(tmpClient);
				}
				if (tmpClient->responseSent())
				{
					// Remove the EPOLLOUT event
					_polling->setClientEPOLLOUT(tmpClient, false);
					tmpClient->refreshClient();
				}
			}
			// tmpClient->printStatus();	
			if (tmpClient->toBeClosed())
			{
				_polling->deleteCLient(tmpClient);
				return ;
			}
		}
		catch (Tools::Exception &e)
		{
			throwHandler(tmpClient, e, config);
		}
	}
	else
	{
		// Keep going boi
	}
	std::clog << tmpClient->getResponseBuff() << std::endl;
	// exit(1);
}

bool ServerManager::matchServerFD(int eventFD) const
{
	if (_servSockFDs.find(eventFD) != _servSockFDs.end())
	{
		std::clog << ORANGE << "matchServerFD new client found from FD " << eventFD << RESET << std::endl;
		return true;
	}
	return false;
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
		const epoll_event *eventArray = _polling->getEventArray();
		for (int i = 0; i < _polling->getEventCount(); i++)
		{
			int eventFD = eventArray[i].data.fd;
			_polling->setCurrEventFD(eventFD);
			if (matchServerFD(eventFD))
				_polling->registerNewClient(eventFD);
			else
				existingClient(eventFD);
		}
	}
}

void ServerManager::mainLoop()
{
	// int i = 0;
	while (!_sigStop)
	{
		try
		{
			eventLoop();
		}
		catch (Tools::Exception &e)
		{
			if (e.getReturnCode() == 0) {
				std::clog << PINK << e.getMsgLog() << RESET << std::endl;
			}
		}
		catch (std::exception &e)
		{
			std::clog << ORANGE << e.what() << RESET << std::endl;
		}
		catch (...)
		{
			std::clog << ORANGE << "Undefined error" << RESET << std::endl;
		}
	}
}
