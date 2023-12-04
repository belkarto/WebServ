#include "Server.hpp"

void	Server::setupServer(void)
{
	struct addrinfo		hints;
	int					status;
	int					reuse = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if ((status = getaddrinfo(node, service, &hints, &bind_addr)))
	{
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
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
	if (bind(sockfd, bind_addr->ai_addr, bind_addr->ai_addrlen))
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(sockfd, 5) < 0)
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
