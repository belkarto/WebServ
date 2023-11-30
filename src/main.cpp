#include "Multiplexer.hpp"
#include "ConfigParser.hpp"
#include <csignal>

int	main(int ac, char **av, char **env)
{
	if (ac > 2)
	{
		std::cerr << "Usage: ./executable [configfile]" << std::endl;
		return 1;
	}
	if (ac > 1)
		webServManager(av[1], env);
	else
		webServManager(DEFAULT_CONF_PATH, env);
	return (0);
}


void	webServManager(const char *config_path, char **env)
{
	std::vector<Server>	servers;

  	signal(SIGPIPE, SIG_IGN);
	try 
	{
		ConfigParser	parser(config_path, servers);
		Multiplexer		multiplexer(servers, env);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
