#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

# define DEFAULT_HOST 					"localhost"
# define DEFAULT_PORT					"1337"
# define DEFAULT_ROOT					"./html"
# define DEFAULT_INDEX					"index.html"
# define DEFAULT_AUTOINDEX				false
# define DEFAULT_CLIENT_MAX_BODY_SIZE	1048576	// 1M


typedef struct Location
{
	std::string					uri;
	std::string					root;
	std::vector<std::string>	index;
	std::vector<std::string>	method;
	std::pair<int, std::string>	redirect;
	bool						autoindex;

}Location;

class Server
{
	public:
		Server();
		struct addrinfo								*bind_addr;
		std::vector<std::string>					server_name;
		std::vector<std::string>					index;
		std::string									root;
		long long									client_max_body_size;
		std::map<std::vector<int>, std::string >	error_page;
		bool										autoindex;
		std::vector<Location>						location;
};
#endif
