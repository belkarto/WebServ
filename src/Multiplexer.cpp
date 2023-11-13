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
	this->connectionListener();
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
			sockfds[it->bind_addr_str] = it->listen_socket;
			epoll_add(epfd, it->listen_socket);
		}
		else
			it->listen_socket = it2->second;
	}
}

void	Multiplexer::registerClient(std::vector<Server>::iterator& serverIt)
{
	Client client;
	serverIt->acceptConnection(client);
	epoll_add2(epfd, client.connect_socket);
	clients.push_back(client);
}

void	Multiplexer::dropClient(std::vector<Client>::iterator& clientIt)
{
	close(clientIt->connect_socket);
	clients.erase(clientIt);
}


void	Multiplexer::sendResponseHeaders(std::vector<Client>::iterator& clientIt)
{
	
}

void	Multiplexer::sendResponse(std::vector<Client>::iterator& clientIt)
{
	
}

void	Multiplexer::connectionListener()
{
	int								i;
	std::vector<Server>::iterator	serverIt;
	std::vector<Client>::iterator	clientIt;

	while (Running)
	{
		
		if ((num_events = epoll_wait(epfd, events, MAX_EVENTS, -1)) < 0)
		{
			perror("epoll_wait()");
			exit(EXIT_FAILURE);
		}
		i = -1;
		while (++i < num_events)
		{
			if ((serverIt = this->findListenSocket(events[i].data.fd, servers)) != servers.end())
				registerClient(serverIt);
			else if ((clientIt = this->findConnectSocket(events[i].data.fd, clients)) != clients.end())
			{
				if ((events[i].events & EPOLLIN))
				{
					if (clientIt->headers_recieved)
					{
						// body ignored in case of GET and DELETE
						// handle POST body
						// TODO:
					}
					else
						getClientHeaders(clientIt);
				}
				if ((events[i].events & EPOLLOUT))
				{
					if (!clientIt->headers_recieved && clientIt->headerTimedout())
					{
						// send 408 error  (Request Time-out)
						// TODO:
						dropClient(clientIt);
					}
					else if (clientIt->headers_recieved)
					{
						// send response in case of a GET request
						// TODO:
						// pay attention to GET request
					}
				}
			}
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

void	Multiplexer::getClientHeaders(std::vector<Client>::iterator& clientIt)
{
	ssize_t				r;
	const char			*delim;
	std::stringstream	ss;
	size_t				pos;

	clientIt->header_buffer = new char[CLIENT_HEADER_BUFFER_SIZE];
	bzero(clientIt->header_buffer, CLIENT_HEADER_BUFFER_SIZE);
	r = recv(clientIt->connect_socket, clientIt->header_buffer, CLIENT_HEADER_BUFFER_SIZE, 0);
	if (r < 1)
		dropClient(clientIt);
	else
	{
		clientIt->headers.append(clientIt->header_buffer, r);
		if (!clientIt->request_line_received)
		{
			if ((pos = clientIt->headers.find(delim)) == std::string::npos)
			{
				// 414 (Request-URI Too Large)
				// TODO:
				dropClient(clientIt);
			}
			else
			{
				ss << clientIt->headers;
				ss >> clientIt->method;
				ss >> clientIt->request_uri;
				// parse method && URI 
				// TODO:
				clientIt->headers = clientIt->headers.substr(pos + sizeof(delim));
				clientIt->request_line_received = true;
			}
		}
		if (!clientIt->headers.empty())
			parseHeaders(clientIt);
		delete[] clientIt->header_buffer;
	}
}


void	Multiplexer::parseHeaders(std::vector<Client>::iterator& clientIt)
{
	size_t	pos, offset;

	offset = 0;
	while ((pos = clientIt->headers.find("\r\n", offset)) != std::string::npos)
	{
		if (pos == offset)	// end of headers
		{
			clientIt->headers_recieved = true;
			clientIt->headers.clear();
			return ;
		}
		offset = pos + 2;
		// set header field;
		// TODO:
	}
	if (!offset && clientIt->headers.length() > CLIENT_HEADER_BUFFER_SIZE)
	{
		// 400 (Bad Request)
		// TODO:
		dropClient(clientIt);
	}
	clientIt->headers = clientIt->headers.substr(offset);
}