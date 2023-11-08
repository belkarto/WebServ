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

int	Multiplexer::dropClient(std::vector<Client>::iterator& clientIt)
{
	close(clientIt->connect_socket);
	clients.erase(clientIt);
	return (RETURN_FAILURE);
}


void	Multiplexer::sendResponseHeaders(std::vector<Client>::iterator& clientIt)
{
	(void)clientIt;
}

void	Multiplexer::sendResponse(std::vector<Client>::iterator& clientIt)
{
	send_response(clientIt);
}

void	Multiplexer::connectionListener()
{
	int								i;
	std::vector<Server>::iterator	serverIt;
	std::vector<Client>::iterator	clientIt;

	while (Running)
	{
		dropInactiveClients();
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
					if (clientIt->headers_all_recieved)
					{
						// TODO:
					}
					else
						getClientRequest(clientIt);
				}
				if ((events[i].events & EPOLLOUT))
				{
					if (clientIt->request_all_processed)
					{
						// TODO:
						if (clientIt->response_all_sent)
							clientIt->resetState();
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

int	Multiplexer::getClientRequest(std::vector<Client>::iterator& clientIt)
{
	ssize_t				r;
	const char			*delim;
	std::stringstream	ss;
	size_t				pos;
	std::string			method, request_uri, protocol_version;

	clientIt->header_buffer = new char[CLIENT_HEADER_BUFFER_SIZE];
	r = recv(clientIt->connect_socket, clientIt->header_buffer, CLIENT_HEADER_BUFFER_SIZE, 0);
	if (r < 1)
		return (dropClient(clientIt));
	else
	{
		delim = "\r\n";
		clientIt->headers.append(clientIt->header_buffer, r);
		if (!clientIt->request_line_received)
		{
			if ((pos = clientIt->headers.find(delim)) == std::string::npos)
			{
				// 414 (Request-URI Too Large)
				// TODO:
				return (dropClient(clientIt));
			}
			else
			{
				ss << clientIt->headers;
				ss >> method >> request_uri >> protocol_version;
				clientIt->fields["method"] = method;
				clientIt->fields["request_uri"] = request_uri;
				clientIt->fields["protocol_version"] = protocol_version;
				clientIt->headers = clientIt->headers.substr(pos + sizeof(delim));
				clientIt->request_line_received = true;
			}
		}
		if (!clientIt->headers.empty())
			getRequestHeaders(clientIt);
		delete[] clientIt->header_buffer;
	}
}


int	Multiplexer::getRequestHeaders(std::vector<Client>::iterator& clientIt)
{
	size_t	pos, offset;

	offset = 0;
	while ((pos = clientIt->headers.find("\r\n", offset)) != std::string::npos)
	{
		if (pos == offset)	// end of headers
		{
			clientIt->headers_all_recieved = true;
			return ;
		}
		offset = pos + 2;
	}
	if (!offset && clientIt->headers.length() > CLIENT_HEADER_BUFFER_SIZE)
	{
		// 400 (Bad Request)
		// TODO:
		return (dropClient(clientIt));
	}
	clientIt->headers = clientIt->headers.substr(offset);
}


void	Multiplexer::dropInactiveClients()
{
	std::vector<Client>::iterator	it;
	time_t							elapsed;

	it = clients.begin();
	for (; it != clients.end(); it++)
	{
		elapsed = time(NULL) - it->last_activity;
		if (it->keepalive_requests && elapsed > KEEPALIVE_TIMEOUT)
		{
			std::cout << "connection timed out" << std::endl;
			dropClient(it);
		}
	}
}