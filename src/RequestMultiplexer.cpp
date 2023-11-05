#include "ConfigManager.hpp"

ConfigManager::ConfigManager(const char *config_path)
{
	try
	{
		ConfigParser	parser(config_path, servers);
		this->createEpollInstance();
		this->fillEpollSet();
		this->eventListener();
	}
	catch(const std::exception& e)
	{
		std::cerr <<"Error: " << e.what() << std::endl;
	}
}

ConfigManager::~ConfigManager()
{
	std::vector<Server>::iterator	it;
	it = servers.begin();
	for (; it != servers.end(); it++)
		freeaddrinfo(it->bind_addr);
}

void	ConfigManager::createEpollInstance()
{
	epfd = epoll_create1(0);
	if (epfd < 0)
	{
		perror("epoll_create1()");
		exit(EXIT_FAILURE);
	}
}

void	ConfigManager::fillEpollSet()
{
	std::map<std::string, int>				sockfds;
	std::vector<Server>::iterator			it;
	std::map<std::string, int>::iterator	it1;

	it = servers.begin();
	for (; it != servers.end(); it++)
	{
		if ((it1 = sockfds.find(it->bind_addr_str)) == sockfds.end())
		{
			it->initServer();
			sockfds[it->bind_addr_str] = it->sockfd;
			epoll_add(epfd, it->sockfd);
		}
		else
			it->sockfd = it1->second;
	}
}


void send_response_headers(Client &client) 
{

	client.sending = true;	// client can recieve response body after recieving headers
	std::cout << itc->headers << std::endl;
	std::stringstream	ss(itc->headers);
	ss >> itc->path;
	itc->path.clear();
	ss >> itc->path;
	std::cout << itc->path << std::endl;
	std::string	fullpath = "www" + client.path;
	// client.response = new std::ifstream("www/html/example.html");
	client.response = new std::ifstream(fullpath.c_str());
	if (!client.response->is_open())
	{
		std::cout << "failed to open file" << std::endl;
		client.sending = false;
		client.recieved = 0;
		client.sent = 0;
		client.response_size = 0;
		client.path.clear();
		client.headers.clear();
		delete client.response;
		std::string response_body = "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>";
		std::stringstream	ss;
		ss << response_body.length();
		std::string content_length = ss.str();
		std::string httpResponse = "HTTP/1.1 404 Not Found\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: " + content_length + "\r\n"
                          "\r\n" + response_body;

		write(client.sockfd, httpResponse.c_str(), httpResponse.size());
		return ;
	}
	// Get file size
	client.response->seekg(0, std::ios::end);
	client.response_size = client.response->tellg();
	client.response->seekg(0, std::ios::beg);

    // Convert the file length to a string
	std::stringstream ss;
	ss << client.response_size;
    std::string content_length = ss.str();

    // Construct the HTTP response with the file content
    std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: " + getMimeType(getFileExtension(fullpath)) + "\r\n"
                              "Connection: keep-alive\r\n"
                              "Content-Length: " + content_length + "\r\n"
                              "\r\n";

    // Send the rheaders to the client
    write(client.sockfd, httpResponse.c_str(), httpResponse.size());
}

void	send_response_body(Client &client)
{
	int	count;

	if (client.sent == client.response_size)
	{
		std::cout << "all data is sent for " << client.sockfd << std::endl;
		client.resetState();
		return ;
	}
	client.response->read(client.buffer, MAX_SIZE);
	count = client.response->gcount();
	client.sent += write(client.sockfd, client.buffer, count);
	std::cout << client.sent << "     " << client.response_size << std::endl;
	if (client.sent == client.response_size)
	{
		std::cout << "all data is sent for " << client.sockfd << std::endl;
		client.resetState();
		return ;
	}
}

void	ConfigManager::eventListener()
{
	struct epoll_event				events[MAX_EVENTS];
	int								num_events;
	int								i;
	std::vector<Server>::iterator	it;
	std::vector<Client>::iterator	itc;
	Client							client;
	int								r;
	while (true)
	{
		if ((num_events = epoll_wait(epfd, events, MAX_EVENTS, -1)) < 0)
		{
			perror("epoll_wait()");
			exit(EXIT_FAILURE);
		}
		i = -1;
		while (++i < num_events)
		{
			if ((it = this->findSocket(events[i].data.fd, servers)) != servers.end())
			{
				std::cout << "\nnew Client connected..." << std::endl;
				it->acceptConnection(client);
				epoll_add2(epfd, client.sockfd);
				clients.push_back(client);
			}
			else
			{
				itc = this->findSocket(events[i].data.fd, clients);

				if (events[i].events & EPOLLIN)
				
				{
					// Client is ready for readness
					r = recv(itc->sockfd, itc->buffer + itc->recieved, MAX_SIZE - itc->recieved, 0);
					std::cout << r << std::endl;
					// keep alive connection
					if (r < 1)
					{
						std::cout << "\nclient disconnected..." << std::endl;
						epoll_delete(epfd, itc->sockfd);
						close(itc->sockfd);
						clients.erase(itc);
					}
					else
					{
						std::cout << "\n" << r << " bytes of request recieved from " << itc->sockfd << std::endl;
						// std::string	headers(itc->buffer, itc->recieved);
						itc->headers.append(itc->buffer + itc->recieved, r);
						itc->recieved += r;
						if (itc->headers.find("\r\n\r\n") != std::string::npos)
						{
							// SENDING Headers
							std::cout << "sending headers..." << std::endl;
	
							send_response_headers(*itc);
						}
					}
				}
				else if ((events[i].events & EPOLLOUT) && itc->sending)
				{
					// sending body
					send_response_body(*itc);
				}
			}
		}
	}
}