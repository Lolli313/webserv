
#include "ServerManager.hpp"
#include "parsing/ParseConfig.hpp"

int _sigStop = 0;

void handle_signals(int sig)
{
	if (sig == SIGINT)
		_sigStop = 1;
}

int main(int ac, char **av) {
	(void)ac;
	(void)av;
	if (ac != 2) {
		std::clog << "Please include one config file" << std::endl;
		return 1;
	}
	std::signal(SIGINT, &handle_signals);
	
	// ParseConfig pc((std::string(av[1])));

	std::vector<std::string> temp;
	temp.push_back("8080");
	try {
		ParseConfig pc((std::string(av[1])));
		pc.printData();
		// ServerManager serverManager(temp);
		// serverManager.mainLoop();
	}
	catch (Tools::Exception& e){
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