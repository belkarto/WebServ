#include "WebServer.hpp"

void    WebServer::readConfig(void)
{
	const char	*arr[] = {
		"listen",
		"server_name",
		"root",
		"index",
		"client_max_body_size",
		"error_page"
		"location"
	};
	std::vector<std::string>	directives(arr, arr +(sizeof(arr) / sizeof(char *)));
	void (Server::*setdirectives[])(std::stringstream&) = {
		&Server::setListen,
		&Server::setServerName,
		&Server::setClientBodyMaxSize,
		&Server::setRoot,
		&Server::setIndex,
		&Server::setErrorPages,
		&Server::setLocations
	};
	std::string line;
	std::ifstream   config(config_file);
	if (!config.is_open())
	   std::cerr << "failed to open config file" << std::endl;
	/* find server context */
	std::stringstream	ss;
	std::string			directive, del, blank;			
	while (std::getline(config, line))
	{

	}
}
