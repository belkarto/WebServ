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

std::vector<Server>::iterator	ConfigManager::findListenSocket(int sockfd)
{
	std::vector<Server>::iterator	it;

	it = servers.begin();
	for (; it != servers.end(); it++)
		if (it->sockfd == sockfd)
			break ;
	return it;
}

std::vector<Client>::iterator	ConfigManager::findConnectSocket(int sockfd)
{
	std::vector<Client>::iterator	it;

	it = clients.begin();
	for (; it != clients.end(); it++)
		if (it->sockfd == sockfd)
			break ;
	return it;
}


void send_response(int sockfd)
{
	std::string response = "HTTP/1.1 200 OK\n";
	response += "Content-Type: text/html\n";
	response += "\n";
	response += "<!DOCTYPE html>\n";
	response += "<html>\n";
	response += "<body>\n";
	response += "<h1>Hello World!</h1>\n";
	response += "</body>\n";
	response += "</html>\n\n\n";
	write(sockfd, response.c_str(), response.size());
}

std::string get_headers(int sockfd)
{
	int		CHUNK = 1024;
	size_t readval;
	char buffer[CHUNK];
	size_t pos;
	std::string s;
	std::string headers;
	std::string body;

	while ((readval = read(sockfd, buffer, CHUNK)) > 0)
	{
		s.append(buffer, readval);
		if ((pos = s.find_last_of("\r\n\r\n")) != std::string::npos)
		{
			headers = s.substr(0, pos);
			break;
		}
	}
	return headers;
}

void	ConfigManager::eventListener()
{
	struct epoll_event				events[MAX_EVENTS];
	int								num_events;
	int								i;
	std::vector<Server>::iterator	it;
	std::vector<Client>::iterator	itc;
	Client							client;

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
			if ((it = this->findListenSocket(events[i].data.fd)) != servers.end())
			{
				std::cout << "new Client connected" << std::endl;
				it->acceptConnection(client);
				epoll_add(epfd, client.sockfd);
				client.read = true;
				client.write = true;
				clients.push_back(client);
			}
			else
			{
				// Client is ready for readness
				itc = this->findConnectSocket(events[i].data.fd);
				if (itc->read)
				{
					std::cout << "read client request" << std::endl;
					get_headers(itc->sockfd);
					itc->read = false;
					
				}
				else if (itc->write)
				{
					std::cout << "response set" << std::endl;
					send_response(itc->sockfd);
					itc->write = false;
					// epoll_delete(epfd, events[i].data.fd);
					// close(events[i].data.fd);
				}
			}
		}
		sleep(2);
		std::cout << "an iteration is done" << std::endl;
	}
}