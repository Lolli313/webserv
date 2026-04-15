
#include "ServerManager.hpp"
#include "parsing/ParseConfig.hpp"
#include "HttpResponse.hpp"

bool g_isChild = false;

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
		LOG(CRITICAL, "Please include one config file");
		return 1;
	}
	std::signal(SIGINT, &handle_signals);

	try {
		ParseConfig pc((std::string(av[1])));
		// pc.printData();
		ServerManager sm(pc.getServerConfig());
		sm.mainLoop();
	}
	catch (Tools::Exception &e) {
		if (e.getReturnCode() == 0)
			LOG(CRITICAL, RED, "Custom exception", e.getMsgLog());
		else if (e.getReturnCode() == 1)
			LOG(CRITICAL, e.getMsgLog());
		else
			LOG(CRITICAL, e.getMsgLog());

	}
	catch (std::exception &e) {
		LOG(CRITICAL, RED, "Builtin exception: ", e.what());
	}
	catch (...) {
		LOG(CRITICAL, "Undefine error");
	}
	return 0;
}
