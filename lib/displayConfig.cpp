#include "Multiplexer.hpp"

void	printstr(std::string param)
{
	std::cout << param << " ";
}

void	printnbr(int nb)
{
	std::cout << nb << " ";
}

void	printpair(std::pair<std::vector<int>, std::string> pr)
{
	std::for_each(pr.first.begin(), pr.first.end(), printnbr);
	std::cout << pr.second << " ";
}

void    printLocation(Location location)
{
	std::cout << "	location {" << std::endl;

	std::cout << "		root " << location.root << " ;" << std::endl;

	std::cout << "		index ";
	std::for_each(location.index.begin(), location.index.end(), printstr);
	std::cout << ";" << std::endl;

	std::cout << "		autoindex " << location.autoindex << " ;" << std::endl;

	std::cout << "		method ";
	std::for_each(location.method.begin(), location.method.end(), printstr);
	std::cout << " ;" << std::endl;

	std::cout << " 		redirect ";
	std::cout << location.redirect << " ;" << std::endl;

	std::cout << "	}" << std::endl;
}

void	printServer(Server& server)
{
	std::cout << "server {" << std::endl;

	std::cout << "	listen " << server.bind_addr_str << " ;" << std::endl;

	std::cout << "	server_name ";
	std::for_each(server.server_name.begin(), server.server_name.end(), printstr);
	std::cout << ";" << std::endl;

	std::cout << "	root " << server.root << " ;" << std::endl;

	std::cout << "	index ";
	std::for_each(server.index.begin(), server.index.end(), printstr);
	std::cout << ";" << std::endl;

	std::cout << "	error_page ";
	std::for_each(server.error_page.begin(), server.error_page.end(), printpair);
	std::cout << ";" << std::endl;

	std::cout << "	client_max_body_size " << server.client_max_body_size << " ;" << std::endl;
	std::cout << "	autoindex " << server.autoindex << " ;" << std::endl;

	std::for_each(server.location.begin(), server.location.end(), printLocation);
	std::cout << "}\n\n\n";
}

void    showServers(std::vector<Server> &servers)
{
    std::cout << servers.size() << std::endl;
	std::for_each(servers.begin(), servers.end(), printServer);
}

void log(const std::string method, const std::string s)
{
    // get time and date
    std::time_t now = time(NULL);
    std::tm    *tm = localtime(&now);
    if (method == "POST")
        std::cout << YELLOW;
    else if (method == "GET")
        std::cout << GREEN;
    else if (method == "DELETE")
        std::cout << BLUE;
    else
        std::cout << RED;
    std::cout << std::setfill('0') << std::setw(4) << tm->tm_year + 1900 << "-" << std::setw(2) << tm->tm_mon + 1 << "-"
              << std::setw(2) << tm->tm_mday << " ";
    std::cout << "[" << std::setw(2) << tm->tm_hour << ":" << std::setw(2) << tm->tm_min << ":" << std::setw(2)
              << tm->tm_sec << "] ==> " << s << RESET << std::endl;
}
