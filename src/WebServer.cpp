#include "WebServer.hpp"


void    WebServer::readConfig(void)
{
	const char	*arr[] = {
		"server",
		"listen",
		"client",
		"root",
		"index",
		"error_page",
		"location"
	};
	std::vector<std::string>	directives(arr, arr + sizeof(arr));
	std::string line;
	std::ifstream   config(config_file);
	if (!config.is_open())
	    throw std::ifstream::failure("failed to open config file");
	while (true)
	{
	    while (std::getline(config, line) && line.empty());
		
	}
}

/*
	two types of blocks => server and location
	each block is delimited by curly braces
	in a block :
		each directive is definded in a signle line and ends with ;
*/