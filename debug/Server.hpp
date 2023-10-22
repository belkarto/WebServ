#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <vector>
#include <sys/select.h>

# define CHUNK 1024

class Server
{
	public:
		const char			*node;
		const char			*service;
		struct addrinfo		*bind_addr;
		int					sockfd;

		Server(const char* node, const char* service);
		void	SetupServer(void);
};
void		send_response(int sockfd);
std::string	get_headers(int sockfd);
#endif