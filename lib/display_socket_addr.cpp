#include "webserv.hpp"

void	display_socket_addr(struct addrinfo *bind_addr)
{
	struct sockaddr_in	*pin;
	void				*addr;
    char				ipstr[INET6_ADDRSTRLEN];
    int					port;

	pin = (struct sockaddr_in *) bind_addr->ai_addr;
	addr = &(pin->sin_addr);
	port = ntohs(pin->sin_port);
	inet_ntop(bind_addr->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
	std::cout << "port: " << port << std::endl;
	std::cout << "host: " << ipstr << std::endl;
}
