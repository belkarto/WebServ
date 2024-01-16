#include "webserv.hpp"

bool	addr_resolver(struct addrinfo **resolved_addr, const char *host, const char *port)
{
	struct addrinfo	hints;
	int status;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	status = getaddrinfo(host, port, &hints, resolved_addr);
	if (status)
		*resolved_addr = NULL;
	return (status == 0);
}