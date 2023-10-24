#include "ConfigManager.hpp"

int	main(int ac, char **av)
{
	(void) ac;
	(void) av;
	ConfigManager	config_manager("config/default.conf");
	return 0;
}
