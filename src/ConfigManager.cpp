#include "ConfigManager.hpp"

ConfigManager::ConfigManager(const char *config_path)
{
	try
	{
		ConfigParser	parser(config_path, servers);
		showServers(servers);
		setupServers();
	}
	catch(const std::exception& e)
	{
		std::cerr <<"Error: " << e.what() << std::endl;
	}
	
}

ConfigManager::~ConfigManager()
{
	std::vector<Server>::iterator	it;
	it = servers.begin();
	for (; it != servers.end(); it++)
		freeaddrinfo(it->bind_addr);
}


void	ConfigManager::setupServers()
{
	std::map<std::string, int>				sockfds;
	std::vector<Server>::iterator			it;
	std::map<std::string, int>::iterator	it1;

	it = servers.begin();
	for (; it != servers.end(); it++)
	{
		if ((it1 = sockfds.find(it->bind_addr_str)) == sockfds.end())
		{
			it->initServer();
			sockfds[it->bind_addr_str] = it->sockfd;
		}
		else
		{
			it->sockfd = it1->second;
		}
	}
}