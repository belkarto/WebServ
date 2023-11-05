#include "webserv.hpp"

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
