
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
		// std::clog << "Please include one config file" << std::endl;
		LOG(CRITICAL, "Please include one config file");
		return 1;
	}
	std::signal(SIGINT, &handle_signals);

	try {
		LOG(DEBUG, YELLOW_BRIGHT, "Index file ignored");
		ParseConfig pc((std::string(av[1])));
		// pc.printData();
		ServerManager sm(pc.getServerConfig());
		sm.mainLoop();
	}
	catch (Tools::Exception &e) {
		if (e.getReturnCode() == 0) {
			// std::clog << "Custom exception: " << PINK << e.getMsgLog() << RESET << std::endl;
			LOG(CRITICAL, PINK, "Custom exception: ", e.getMsgLog());
		}
		else if (e.getReturnCode() == 1) {
			// std::clog << ORANGE << e.getMsgLog() << RESET << std::endl;
			LOG(CRITICAL, e.getMsgLog());
		}
		else {
			// std::clog << ORANGE << e.getMsgLog() << RESET << std::endl;
			LOG(CRITICAL, e.getMsgLog());
		}
	}
	catch (std::exception &e) {
		// std::clog << "Builtin exception: " << ORANGE << e.what() << RESET << std::endl;
		LOG(CRITICAL, PINK, "Builting exception: ", e.what());
	}
	catch (...) {
		// std::clog << ORANGE << "Undefined error" << RESET << std::endl;
		LOG(CRITICAL, "Undefine error");
	}
	return 0;
}
