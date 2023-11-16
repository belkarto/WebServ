#include <iostream>
#include <sstream>
#include <algorithm>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <map>

int	not_space(int c)
{
	return !isspace(c);
}

void	trim(std::string &str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), not_space));
	str.erase(std::find_if(str.rbegin(), str.rend(), not_space).base(), str.end());
}

bool	addr_resolver(struct addrinfo **resolved_addr, const char *host, const char *port)
{
	struct addrinfo	hints;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int status;
	return ((status = getaddrinfo(host, port, &hints, resolved_addr)) == 0);
}

std::string	socket_addr_ntop(struct addrinfo *bind_addr)
{
	struct sockaddr_in	*pin;
	void				*addr;
    char				ipstr[INET6_ADDRSTRLEN];
    int					port;
	std::stringstream 	ss;
	std::string			p;

	pin = (struct sockaddr_in *) bind_addr->ai_addr;
	addr = &(pin->sin_addr);
	port = ntohs(pin->sin_port);
	inet_ntop(bind_addr->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
    ss << port;
	p = "";
	p.append(ipstr).append(":").append(ss.str());
	return (p);
}

int main(int ac, char **av)
{
 	std::map<int, std::string> myMap;

    // Inserting key-value pairs into the map
    myMap[1] = "One";
    myMap[2] = "Two";

    // Inserting a key that is already present
    myMap.insert(std::make_pair(1, "Updated One"));
	std::cout << myMap[1] << std::endl;
}