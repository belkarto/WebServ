#include "Multiplexer.hpp"
#include "ConfigParser.hpp"


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
	catch (ConfigFileParsingException &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
	freeServers(servers);
}

void	freeServers(std::vector<Server> &servers)
{
	SERVIT	it, ite;

	it = servers.begin();
	ite = servers.end();
	for (; it != ite; it++)
		if (it->bind_addr)
			freeaddrinfo(it->bind_addr);

}
