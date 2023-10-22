#include "Server.hpp"

void	Server::setupServer(void)
{
	struct addrinfo		hints;
	int					status;
	int					reuse;

	std::cout << "Configuring socket..." << std::endl;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if ((status = getaddrinfo(node, service, &hints, &bind_addr)))
	{
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	std::cout << "creating socket..." << std::endl;
	if ((sockfd = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol)) < 0)
	{
		perror("socket()");
		exit(EXIT_FAILURE);		
	}
	reuse = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt()");
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

Server::Server(const char* node, const char* service)
{
	this->node = node;
	this->service = service;
	this->setupServer();
}