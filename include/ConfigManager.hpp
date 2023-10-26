#ifndef CONFIGMANAGER_HPP
# define CONFIGMANAGER_HPP

#include "webserv.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"

class ConfigManager
{
	std::vector<Server>	servers;
	// std::vector<Client>	clients;

	public:
		ConfigManager(const char *config_path);
};
#endif