#include "Server.hpp"

Server::Server()
{
    root 					= DEFAULT_ROOT;
    client_max_body_size	= DEFAULT_CLIENT_MAX_BODY_SIZE;
    autoindex 				= DEFAULT_AUTOINDEX;
    addr_resolver(&bind_addr, DEFAULT_HOST, DEFAULT_PORT);
    index.push_back(DEFAULT_INDEX);
}


void	Server::emplaceBackLocation()
{
	Location	location;

	location.root 		= this->root;
    location.index 		= this->index;
    location.autoindex 	= this->autoindex;
	location.method.push_back("GET");
	this->location.push_back(location);
}

void    Server::initServer()
{
	int					reuse = 0;
	if ((listen_socket = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol)) < 0)
	{
		perror("socket()");
		exit(EXIT_FAILURE);		
	}
    
	reuse = 1;
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt()");
		exit(EXIT_FAILURE);
	}

	if (bind(listen_socket, bind_addr->ai_addr, bind_addr->ai_addrlen))
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	if (listen(listen_socket, BACKLOG) < 0)
	{
		perror("listen()");
		exit(EXIT_FAILURE);
	}	
  std::cout << "listen on " << bind_addr_str << std::endl;
}


void	Server::acceptConnection(Client &client)
{
	if ((client.connect_socket = accept(listen_socket, NULL, NULL)) < 0)
	{
		perror("accept()");
		exit(EXIT_FAILURE);
	}
	client.listen_socket = this->listen_socket;
}
