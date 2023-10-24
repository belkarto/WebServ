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
void	sig_handler(int signum);
void    split(std::vector<std::string>& fields, std::stringstream& ss);
void	addr_resolver(struct addrinfo **resolved_addr, std::string& addr, std::string& port);
void    check_directive_syntax(std::vector<std::string>& fields);
size_t	ft_stoul(const char* str);
size_t	convert_to_bytes(size_t size, char unit);

#endif