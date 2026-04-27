
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

void printTitle()
{
    std::cout << PINK << "                      __                                       \n";
    std::cout << "                     /\\ \\                                      \n";
    std::cout << "     __  __  __     __\\ \\ \\____    ____     __   _ __   __  __ \n";
    std::cout << "    /\\ \\/\\ \\/\\ \\  /'__`\\ \\ '__`\\  /',__\\  /'__`\\/\\`'__\\/\\ \\/\\ \\\n";
    std::cout << "    \\ \\ \\_/ \\_/ \\/\\  __/\\ \\ \\L\\ \\/\\__, `\\/\\  __/\\ \\ \\/ \\ \\ \\_/ |\n";
    std::cout << "     \\ \\___x___/'\\  \\____\\\\ \\_,__/\\/\\____/\\ \\____\\\\ \\_\\  \\ \\___/ \n";
    std::cout << "      \\/__//__/   \\/____/ \\/___/  \\/___/  \\/____/ \\/_/   \\/__/  \n";
	std::cout << std::endl;
	std::cout << "  by:      " << PURPLE << "    aakerblo    " << GREEN << "   tcardair    " << LIGHT_BLUE << "   chboegne    ";
	std::cout << std::endl;
	std::cout << RESET << std::endl;
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
		printTitle();
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
		LOG(CRITICAL, "Builtin exception: ", e.what());
	}
	catch (...) {
		LOG(CRITICAL, "Undefined error");
	}
	return 0;
}
