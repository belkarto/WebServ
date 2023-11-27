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
# include <sys/stat.h>
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
# include <sys/epoll.h>
# include <dirent.h>
# include "define.hpp"
# include "Exception.hpp"

class Server;
class Client;
class Multiplexer;

/*					main										*/
void				webServManager(const char *config_path);

/*                  lib                                         */
void            split(std::vector<std::string>& fields, std::stringstream& ss);
void                split(std::vector<std::string>& params, std::string str);
void	            trim(std::string &str);
long long	    	ft_stoll(const char* str);
int			    	ft_stoi(const char* str);
long long	    	converttobytes(long long size, char unit);
int	            	not_digit(char c);
int					isunit(int unit);
bool            	addr_resolver(struct addrinfo **resolved_addr, const char *host, const char *port);
std::string	    	socket_addr_ntop(struct addrinfo *bind_addr);
void	        	epoll_add(int epfd, int fd);
void	        	epoll_add2(int epfd, int fd);
void	        	epoll_delete(int epfd, int fd);
std::streamsize		getFileSize(std::ifstream *file);
bool				is_directory(const char* path);

/*                  debug                                       */
void        		showServers(std::vector<Server> &servers);

/*                  typedef                                       */
typedef std::vector<Client>  			    	CLIENTVECT;
typedef std::vector<Server> 			    	SERVVECT;
typedef std::vector<Client>::iterator	    	CLIENTIT;
typedef std::vector<Server>::iterator	    	SERVIT;
typedef std::vector<std::string>		   	    STRINGVECT;
typedef std::vector<std::string>::iterator	    STRINGVECTIT;
typedef std::vector<int>                        INTVECT;
typedef std::vector<struct Location>::iterator	LOC_VECT_IT;
#endif
