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
# include <climits>
# include "define.hpp"
# include <sys/epoll.h>
# include "Exception.hpp"

class Server;


/*                  lib                                         */
void		split(std::vector<std::string>& fields, std::stringstream& ss);
long long	ft_stoll(const char* str);
int			ft_stoi(const char* str);
long long	converttobytes(long long size, char unit);
int			isunit(int unit);
bool        addr_resolver(struct addrinfo **resolved_addr, const char *host, const char *port);
std::string	socket_addr_ntop(struct addrinfo *bind_addr);
void	    epoll_add(int epfd, int fd);
void	    epoll_delete(int epfd, int fd);

/*                  debug                                       */
void        showServers(std::vector<Server> &servers);
#endif
