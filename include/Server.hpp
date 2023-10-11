#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

typedef struct location
{
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::vector<std::string>	allowed_methods;
	std::pair<int, std::string>	redirection;
}location;

class Server
{
	public:
		Server(void);
		std::pair<std::string, int>					listen;
		std::string									server_name;
		size_t										client_body_max_size;
		std::string									root;
		std::string									index;
		std::vector<std::pair<int, std::string> >	error_pages;
		std::vector<struct location>				locations;

};
#endif
