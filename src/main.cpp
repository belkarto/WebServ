#include "webserv.hpp"

int	main(int ac, char **av)
{
	WebServer	webserv;
	if (ac < 2)
		webserv.config_file = "ressources/config/server.conf";
	else
		webserv.config_file = av[1];
	webserv.readConfig();
	return 0;
}
