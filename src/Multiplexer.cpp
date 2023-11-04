#include "Multiplexer.hpp"

Multiplexer::Multiplexer(std::vector<Server> &servers) : servers(servers)
{
	epfd = epoll_create1(0);
	if (epfd < 0)
	{
		perror("epoll_create1()");
		exit(EXIT_FAILURE);
	}
	this->registerServers();
	this->connectionListner();
}

Multiplexer::~Multiplexer()
{
	std::vector<Server>::iterator	serverIt;

	serverIt = servers.begin();
	for (; serverIt != servers.end(); serverIt++)
	{
		freeaddrinfo(serverIt->bind_addr);
		close(serverIt->listen_socket);
	}
}

void	Multiplexer::registerServers()
{
	std::map<std::string, int>				sockfds;
	std::vector<Server>::iterator			it;
	std::map<std::string, int>::iterator	it2;

	it = servers.begin();
	for (; it != servers.end(); it++)
	{
		if ((it2 = sockfds.find(it->bind_addr_str)) == sockfds.end())
		{
			it->initServer();
			std::cout << socket_addr_ntop(it->bind_addr) <<  " is running ..." << std::endl;
			sockfds[it->bind_addr_str] = it->listen_socket;
			epoll_add(epfd, it->listen_socket);
		}
		else
			it->listen_socket = it2->second;
	}
}

void	Multiplexer::registerClients(std::vector<Server>::iterator& serverIt)
{
	std::cout << "new connection established" << std::endl;
	Client client;
	serverIt->acceptConnection(client);
	epoll_add2(epfd, client.connect_socket);
	clients.push_back(client);
}

void	Multiplexer::getClientRequest(std::vector<Client>::iterator& clientIt)
{
	ssize_t	r;

	std::cout << "getting client request" << std::endl;
	// r = recv(clientIt->connect_socket, clientIt->buffer + clientIt->recieved, MAX_SIZE - clientIt->recieved, 0);
	r = recv(clientIt->connect_socket, clientIt->buffer, MAX_SIZE, 0);
	if (r < 1)
	{
		std::cout << "client disconnected " << clientIt->connect_socket << " " << clientIt->listen_socket << std::endl;
		close(clientIt->connect_socket);
		clients.erase(clientIt);
	}
	else
	{
		// clientIt->headers.append(clientIt->buffer + clientIt->recieved, r);
		// clientIt->recieved += r;
		clientIt->headers.append(clientIt->buffer, r);
		clientIt->recieved = r;
		if (clientIt->headers.find("\r\n\r\n") != std::string::npos)
			this->sendResponseHeaders(clientIt);
	}
}

void	Multiplexer::sendResponseHeaders(std::vector<Client>::iterator& clientIt)
{
	// std::cout << "sending response headers" << std::endl;
	send_response_headers(*clientIt);
}

void	Multiplexer::sendResponse(std::vector<Client>::iterator& clientIt)
{
	// std::cout << "sending response body" << std::endl;
	if (clientIt->sending)
		send_response_body(*clientIt);
}

void	Multiplexer::connectionListner()
{
	int								i;
	std::vector<Server>::iterator	serverIt;
	std::vector<Client>::iterator	clientIt;

	while (Running)
	{
		std::cout << "waiting for incomming connections...." << std::endl;
		if ((num_events = epoll_wait(epfd, events, MAX_EVENTS, -1)) < 0)
		{
			perror("epoll_wait()");
			exit(EXIT_FAILURE);
		}
		// in case of a time out => reset the clients queue;
		i = -1;
		while (++i < num_events)
		{
			if ((serverIt = this->findListenSocket(events[i].data.fd, servers)) != servers.end())
				registerClients(serverIt);
			else if ((clientIt = this->findConnectSocket(events[i].data.fd, clients)) != clients.end())
			{
				if ((events[i].events & EPOLLIN))
				{
					std::cout << "client " << clientIt->path << " ready for readness i: " << i << std::endl;
					getClientRequest(clientIt);
				}
				if ((events[i].events & EPOLLOUT))
				{
					std::cout << "client " << clientIt->path << "ready for writeness i: " << i << std::endl;
					sendResponse(clientIt);
					
				}
				if ((time(NULL) - clientIt->last_activity) > TIMEOUT)
				{
					close(clientIt->connect_socket);
					clients.erase(clientIt);
				}
			}
			// sleep(4);
		}
	}
}


std::vector<Server>::iterator	Multiplexer::findListenSocket(int socket, std::vector<Server> &sockets)
{
	std::vector<Server>::iterator	serverIt;

	serverIt = sockets.begin();
	while (serverIt != sockets.end() && serverIt->listen_socket != socket)
		serverIt++;
	return serverIt;
}

std::vector<Client>::iterator	Multiplexer::findConnectSocket(int socket, std::vector<Client> &sockets)
{
	std::vector<Client>::iterator	clientIt;

	clientIt = sockets.begin();
	while (clientIt != sockets.end() && clientIt->connect_socket != socket)
		clientIt++;
	return clientIt;
}