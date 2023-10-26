#include "webserv.hpp"

bool	addr_resolver(struct addrinfo **resolved_addr, const char *host, const char *port)
{
	struct addrinfo	hints;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int status;
	return ((status = getaddrinfo(host, port, &hints, resolved_addr)) == 0);
}