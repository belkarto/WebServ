#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

class Server
{
	public:
		struct addrinfo								*bind_addr;
		std::vector<std::string>					server_name;
		size_t										client_body_max_size;
		std::string									root;
		std::vector<std::string>					index;
		std::map<std::vector<int>, std::string >	error_pages;
		bool						autoindex;

};
#endif
