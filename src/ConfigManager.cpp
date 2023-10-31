#include "ConfigManager.hpp"

ConfigManager::ConfigManager(const char *config_path)
{
	try
	{
		ConfigParser	parser(config_path, servers);
		showServers(servers);
	}
	catch(const std::exception& e)
	{
		std::cerr <<"Error: " << e.what() << std::endl;
	}
	
}
