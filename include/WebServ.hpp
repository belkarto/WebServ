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
# include <utility>
# include "Client.hpp"
# include "Server.hpp"
# include "WebServer.hpp"

void	sig_handler(int signum);
#endif