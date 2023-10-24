#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

class Server
{
	public:
		struct addrinfo								*bind_addr;
		std::vector<std::string>					server_name;
		std::vector<std::string>					index;
		std::string									root;
		long long									client_max_body_size;
		std::map<std::vector<int>, std::string >	error_page;
		bool										autoindex;

};
#endif
