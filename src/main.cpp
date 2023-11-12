#include "Multiplexer.hpp"
#include "ConfigParser.hpp"

int	main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cerr << "Usage: ./executable [configfile]" << std::endl;
		return 1;
	}
	if (ac > 1)
		webServManager(av[1]);
	else
		webServManager(DEFAULT_CONF_PATH);
	return (0);
}


void	webServManager(const char *config_path)
{
	std::vector<Server>	servers;

	try 
	{
		ConfigParser	parser(config_path, servers);
		Multiplexer		multiplexer(servers);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}