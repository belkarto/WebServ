#include "ConfigManager.hpp"

int	_main()
{
	ConfigManager	config_manager("config/default.conf");
	return 0;
}

int	main(int ac, char **av)
{
	(void) ac;
	(void) av;
	_main();
	// system("leaks webserv");
}
