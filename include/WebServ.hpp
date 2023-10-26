#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <unistd.h>
# include <signal.h>
# include <arpa/inet.h>
# include <string.h>
# include <fcntl.h>
# include <fstream>
# include <sstream>
# include <utility>
# include <algorithm>
# include <vector>
# include <queue>
# include <map>
# include <exception>
# include <cstdlib>
# include "Exception.hpp"


/*                  lib                         */

void		split(std::vector<std::string>& fields, std::stringstream& ss);
long long	ft_stoll(const char* str);
int			ft_stoi(const char* str);
long long	converttobytes(long long size, char unit);
int			isunit(int unit);
bool        addr_resolver(struct addrinfo **resolved_addr, const char *host, const char *port);
void	    display_socket_addr(struct addrinfo *bind_addr);
#endif