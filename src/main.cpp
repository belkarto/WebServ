#include "ConfigManager.hpp"


int	main(int ac, char **av)
{
	if (ac < 2)
		ConfigManager	config_manager(DEFAULT_CONF_PATH);
	else if (ac == 2)
		ConfigManager	config_manager(av[1]);
	// else 
		// error
}
