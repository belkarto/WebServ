#include "Server.hpp"


int	main(void)
{

	int					new_sockfd;
	struct sockaddr_in	client_addr;
	socklen_t			client_len;
	std::vector<Server> servers;
	std::vector<int>	clients;

	servers.emplace_back("localhost", "1024");
	servers.emplace_back("127.0.0.8", "1025");
	servers.emplace_back("10.12.3.1", "1026");
	while (true)
	{
		std::cout << "waiting for connection.." << std::endl;
		if ((new_sockfd = accept(servers.begin()->sockfd, (sockaddr *) &client_addr, &client_len)) < 0)
		{
			perror("accept()");
			exit(EXIT_FAILURE);
		}
		std::cout << "connection established" << std::endl;
		clients.push_back(new_sockfd);
		std::vector<int>::iterator	it  = clients.begin();
		for (; it != clients.end(); it++)
		{
			pid_t pid = fork();
			if (pid == 0)
			{
				close(sockfd);
				std::cout << "sending..." << std::endl;
				send_response(new_sockfd);
				std::cout << "sent" << std::endl;
				std::cout << "closing connection..." << std::endl;
				close(new_sockfd);
				exit(0);
			}
		}
		close(new_sockfd);
	}
	// close(sockfd);
	// freeaddrinfo(res);
    return 0;
}

std::string	get_headers(int sockfd)
{
	size_t		readval;
	char		buffer[CHUNK];
	size_t		pos;
	std::string	s;
	std::string	headers;
	std::string	body;

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

void	send_response(int sockfd)
{
	std::string    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "\r\n";
    response += "<!DOCTYPE html>\r\n";
    response += "<html>\r\n";
    response += "<body>\r\n";
    response += "<h1>Hello World!</h1>\r\n";
    response += "</body>\r\n";
    response += "</html>\r\n";	
    write(sockfd, response.c_str(), response.size());
}