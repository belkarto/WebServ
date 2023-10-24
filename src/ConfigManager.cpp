#include "ConfigManager.hpp"

void	print1(std::string param)
{
	std::cout << param << " ";
}

void	print4(int nb)
{
	std::cout << nb << " ";
}

void	print3(std::pair<std::vector<int>, std::string> pr)
{
	std::for_each(pr.first.begin(), pr.first.end(), print4);
	std::cout << pr.second << " ";
}

void	print2(Server& server)
{
	std::cout << "server {" << std::endl;
	std::cout << "	index ";
	std::for_each(server.index.begin(), server.index.end(), print1);
	std::cout << ";" << std::endl;
	std::cout << "	server_name ";
	std::for_each(server.server_name.begin(), server.server_name.end(), print1);
	std::cout << ";" << std::endl;
	std::cout << "	error_page ";
	std::for_each(server.error_page.begin(), server.error_page.end(), print3);
	std::cout << ";" << std::endl;
	std::cout << "	root " << server.root << " ;" << std::endl;
	std::cout << "	client_max_body_size " << server.client_max_body_size << " ;" << std::endl;
	std::cout << "	autoindex " << server.autoindex << " ;" << std::endl;
	std::cout << "}\n\n\n";
}

void    showServers(std::vector<Server> &servers)
{
    std::cout << servers.size() << std::endl;
	std::for_each(servers.begin(), servers.end(), print2);
}

ConfigManager::ConfigManager(const char *config_path)
{
	try
	{
		ConfigParser	parser(config_path, servers);
		// showServers(servers);
	}
	catch(const std::exception& e)
	{
		std::cerr <<"Error: " << e.what() << std::endl;
	}
	
}
