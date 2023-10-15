#include "webserv.hpp"

int	main(int ac, char **av)
{
	WebServer	webserv;
	if (ac < 2)
		webserv.config_file = "ressources/config/server.conf";
	else
		webserv.config_file = av[1];
	signal(SIGINT, sig_handler);
	try
	{
		webserv.readConfig();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}
