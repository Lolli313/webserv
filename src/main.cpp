
#include "Server.hpp"

int main(int ac, char **av) {
	(void)ac;
	(void)av;
	std::vector<std::string> temp;
	temp.push_back("8080");
	try {
		Server server(temp);
		server.mainLoop();
	}
	catch (Tools::Exception& e){
		if (e.getReturnCode() == 0)
			std::clog << "Custom exception: " << PINK << e.getMsgLog() << RESET << std::endl;
	}
	catch (std::exception &e)
	{
		std::clog << "Builtin exception: " << ORANGE << e.what() << RESET << std::endl;
	}
	catch (...)
	{
		std::clog << RED << "Undefined error" << RESET << std::endl;
	}
	return 0;
}