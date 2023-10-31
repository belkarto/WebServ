#include "Server.hpp"

Server::Server()
{
    addr_resolver(&bind_addr, DEFAULT_HOST, DEFAULT_PORT);
    index.push_back(DEFAULT_INDEX);
    root = DEFAULT_ROOT;
    client_max_body_size = DEFAULT_CLIENT_MAX_BODY_SIZE;
    autoindex = DEFAULT_AUTOINDEX;
}

void    Server::initServer()
{
	std::cout << "creating socket..." << std::endl;
	if ((sockfd = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol)) < 0)
	{
		perror("socket()");
		exit(EXIT_FAILURE);		
	}
    
	std::cout << "binding socket..." << std::endl;
	if (bind(sockfd, bind_addr->ai_addr, bind_addr->ai_addrlen))
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	std::cout << "listening socket..." << std::endl;
	if (listen(sockfd, 10) < 0)
	{
		perror("listen()");
		exit(EXIT_FAILURE);
	}	
}