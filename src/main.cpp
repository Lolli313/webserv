
#include "ServerManager.hpp"
#include "parsing/ParseConfig.hpp"
#include "HttpResponse.hpp"

int _sigStop = 0;

void handle_signals(int sig)
{
	if (sig == SIGINT)
		_sigStop = 1;
}

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	if (ac != 2)
	{
		std::clog << "Please include one config file" << std::endl;
		return 1;
	}
	std::signal(SIGINT, &handle_signals);

	// ParseConfig pc((std::string(av[1])));

	std::vector<std::string> temp;
	temp.push_back("8080");
	try
	{
		// ParseConfig pc((std::string(av[1])));
		// pc.printData();
		// ServerManager serverManager(temp);
		// serverManager.mainLoop();
		std::pair<std::string, std::string> input;
		std::vector<std::pair<std::string, std::string> > respMap;
		input = std::make_pair("arttu", "meh");
		respMap.push_back(input);
		input = std::make_pair("thomas", "yeah");
		respMap.push_back(input);
		input = std::make_pair("Content-type", "yummy");
		respMap.push_back(input);
		HttpResponse response(404, "this is a test");
		response.setResponseHeaders(respMap);
		std::ifstream file("objects/body.txt");
		if (!file)
			throw Tools::Exception("nein");
		std::ostringstream body;
		body << file.rdbuf();
		response.setBody(body.str());
		std::cout << response.getFinalResponse() << std::endl;
	}
	catch (Tools::Exception &e)
	{
		if (e.getReturnCode() == 0)
			std::clog << "Custom exception: " << PINK << e.getMsgLog() << RESET << std::endl;
		else if (e.getReturnCode() == 1)
			std::clog << ORANGE << e.getMsgLog() << RESET << std::endl;
		else
			std::clog << ORANGE << e.getMsgLog() << RESET << std::endl;
	}
	catch (std::exception &e)
	{
		std::clog << "Builtin exception: " << ORANGE << e.what() << RESET << std::endl;
	}
	catch (...)
	{
		std::clog << ORANGE << "Undefined error" << RESET << std::endl;
	}
	return 0;
}