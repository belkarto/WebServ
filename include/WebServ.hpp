
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <unistd.h>
# include <signal.h>
# include <arpa/inet.h>
# include <string.h>
# include <vector>
# include <queue>
# include <fcntl.h>
# include <fstream>
# include <sstream>

# define CHUNK 1024

class Client;

typedef struct Socket
{
	int					fd;
	struct sockaddr_in	addr;
	socklen_t			addrlen;    
}Socket;

# include "Client.hpp"
# include "Server.hpp"

void	sig_handler(int signum);
#endif