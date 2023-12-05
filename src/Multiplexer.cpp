#include "../include/Multiplexer.hpp"

char	**Multiplexer::env;

std::map<std::string, std::string> Multiplexer::mime_types;

std::map<int, std::string> Multiplexer::defErrorPages;

int Multiplexer::keepalive_connections = 0;

const char *Multiplexer::defErrorPagesStrings[NUM_DEF_ERROR] = {
    STATUS_100, STATUS_101, STATUS_200, STATUS_201, STATUS_202, STATUS_203,
    STATUS_204, STATUS_205, STATUS_206, STATUS_300, STATUS_301, STATUS_302,
    STATUS_303, STATUS_304, STATUS_305, STATUS_306, STATUS_307, STATUS_400,
    STATUS_401, STATUS_402, STATUS_403, STATUS_404, STATUS_405, STATUS_406,
    STATUS_407, STATUS_408, STATUS_409, STATUS_410, STATUS_411, STATUS_412,
    STATUS_413, STATUS_414, STATUS_415, STATUS_416, STATUS_417, STATUS_500,
    STATUS_501, STATUS_502, STATUS_503, STATUS_504, STATUS_505
};

const char *Multiplexer::fields[HEADERS_FIELDS_SIZE] = {
	"host",
	"content-type",
	"content-length",
	"connection",
	"transfer-encoding",
};

void (Client::*Multiplexer::fields_setters[HEADERS_FIELDS_SIZE])(std::string &field) = {
	&Client::setHost,
	&Client::setContentType,
	&Client::setContentLength,
	&Client::setConnection,
	&Client::setTransferEncoding,
};

Multiplexer::Multiplexer(SERVVECT &servers, char **env) : servers(servers)
{
	this->env = env;
	epfd = epoll_create1(0);
	if (epfd < 0)
	{
		perror("epoll_create1()");
		exit(EXIT_FAILURE);
	}
	headers_fields.assign(fields, fields + HEADERS_FIELDS_SIZE);
	this->loadMimeTypes();
    this->loadDefErrorPages();
	this->registerServers();
	this->connectionListener();
}

Multiplexer::~Multiplexer()
{
	SERVIT serverIt;

	serverIt = servers.begin();
	for (; serverIt != servers.end(); serverIt++)
	{
		freeaddrinfo(serverIt->bind_addr);
		close(serverIt->listen_socket);
	}
}

void Multiplexer::registerServers()
{
	std::map<std::string, int> sockfds;
	std::map<std::string, int>::iterator it2;
	SERVIT it;

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

void Multiplexer::registerClient(SERVIT &serverIt)
{
	Client client;
	serverIt->acceptConnection(client);
	if (client.connect_socket < 0)
		return ;
	client.serverIt = serverIt;
	epoll_add2(epfd, client.connect_socket);
	clients.push_back(client);
}

void Multiplexer::dropClient(CLIENTIT &clientIt)
{
	close(clientIt->connect_socket);
	clients.erase(clientIt);
	// if (Multiplexer::keepalive_connections > 0)
	// 	Multiplexer::keepalive_connections--;
}


void Multiplexer::connectionListener()
{
	int i;
	SERVIT serverIt;
	CLIENTIT clientIt;

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
					clientIt->active = true;
					if (!clientIt->request_line_received)
						clientIt->header_timeout = time(NULL);
					if (clientIt->headers_all_recieved)
					{
						// TODO:
                        // in case of POST request get the request body
                        clientIt->request_all_processed = true;
					}
					else
						getClientRequest(clientIt);
				}
				if ((events[i].events & EPOLLOUT))
				
					handleResponse(clientIt);
			}
		}
	}
}

void Multiplexer::handleResponse(CLIENTIT &clientIt)
{
	if (clientIt->request_all_processed)
	{
		if (clientIt->response_all_sent)
		{
			if (clientIt->response.connection == "close")
				dropClient(clientIt);
			else
				clientIt->resetState();	// keep-alive connection
		}
		else if (clientIt->response.cgi)
		{
			if (!clientIt->start_responding)
				clientIt->response.checkCgiTimeout(clientIt);
			else
				clientIt->response.sendResponseBuffer(clientIt);
		}
		else if (!clientIt->start_responding)
		{
			if (clientIt->fields["method"] == "GET")
				clientIt->response.setGetResponse(clientIt);
			else if (clientIt->fields["method"] == "POST")
				clientIt->response.setPostResponse(clientIt);
			else if (clientIt->fields["method"] == "DELETE")
				clientIt->response.setDeleteResponse(clientIt);
		}
		else
			clientIt->response.sendResponseBuffer(clientIt);
	}
}


SERVIT Multiplexer::findListenSocket(int socket, SERVVECT &sockets)
{
	SERVIT serverIt;

	serverIt = sockets.begin();
	while (serverIt != sockets.end() && serverIt->listen_socket != socket)
		serverIt++;
	return serverIt;
}

CLIENTIT Multiplexer::findConnectSocket(int socket, CLIENTVECT &sockets)
{
	CLIENTIT clientIt;

	clientIt = sockets.begin();
	while (clientIt != sockets.end() && clientIt->connect_socket != socket)
		clientIt++;
	return clientIt;
}

void Multiplexer::getClientRequest(CLIENTIT &clientIt)
{
	ssize_t 	r;
	try
	{
		clientIt->header_buffer = new char[CLIENT_HEADER_BUFFER_SIZE];
		r = recv(clientIt->connect_socket, clientIt->header_buffer, CLIENT_HEADER_BUFFER_SIZE, 0);
		//TODO:
		if (r < 1)
			return (dropClient(clientIt));
		else
		{
			clientIt->headers.append(clientIt->header_buffer, r);
			if (!clientIt->request_line_received)
				parseRequestLine(clientIt);
			if (!clientIt->headers.empty())
				parseRequestHeaders(clientIt);
		}
	}
	catch(RequestParsingException& e)
	{
		clientIt->request_all_processed = true;
		clientIt->response.status = e.what();
		clientIt->response.setErrorResponse(clientIt);
	}
    if (clientIt->fields["method"] != "POST")
	    delete[] clientIt->header_buffer;
}

bool	checkClientTimeOut(Client client)
{
	// return ((!client.headers_all_recieved 
	// 	&& time(NULL) - client.header_timeout >= CLIENT_HEADER_timeout) ||
	// 		(client.request_all_processed && !client.start_responding 
	// 	&& (Multiplexer::keepalive_connections == KEEPALIVE_CONNECTIONS || time(NULL) - client.last_activity >= KEEPALIVE_TIMEOUT)));

	return (!client.active && time(NULL) - client.last_activity >= KEEPALIVE_TIMEOUT);
}

void Multiplexer::dropInactiveClients()
{
	CLIENTIT newEnd, end, temp;

	newEnd = std::remove_if(clients.begin(), clients.end(), checkClientTimeOut);
	end = clients.end();
	temp = newEnd;
	for (; newEnd != end; newEnd++)
	{
		close(newEnd->connect_socket);
		std::cout << "dropped" << std::endl;
		// if (Multiplexer::keepalive_connections > 0)
		// 	Multiplexer::keepalive_connections--;
	}
	clients.erase(temp, end);
}

void Multiplexer::loadMimeTypes()
{
	std::string line, key, value;
	std::stringstream ss;
	std::ifstream infile(MIMETYPE_PATH);

	if (!infile.is_open())
		return;
	while (getline(infile, line))
	{
		ss << line;
		ss >> key;
		ss >> value;
		mime_types[key] = value;
		ss.str("");
	}
}

void Multiplexer::loadDefErrorPages()
{
    std::stringstream ss;
    int errPageIndex = 0;

    for (int i = 0; i < NUM_DEF_ERROR; i++)
    {
        ss.str(defErrorPagesStrings[i]);
        ss >> errPageIndex;
        this->defErrorPages[errPageIndex] = defErrorPagesStrings[i];
        ss.clear();
    }
}
