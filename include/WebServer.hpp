#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "webserv.hpp"

class WebServer
{
	public:
		const char*			config_file;
		std::vector<Server>	servers;
		std::vector<Client>	clients;

		void	readConfig();
};
#endif