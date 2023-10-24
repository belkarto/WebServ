#include "Server.hpp"

bool	isserver(int fd, std::vector<Server>& servers);

int	main(void)
{

	int					new_sockfd;
	std::vector<Server> servers;
	fd_set				current_fds, ready_fds;
	int					i;

	servers.emplace_back("localhost", "1026");
	servers.emplace_back("127.0.0.1", "1025");
	servers.emplace_back("10.12.8.5", "1026");

	FD_ZERO(&current_fds);
	FD_SET(servers[0].sockfd, &current_fds);
	FD_SET(servers[1].sockfd, &current_fds);
	FD_SET(servers[2].sockfd, &current_fds);

	while (true)
	{
		ready_fds = current_fds;
		int	ready_size;
		if ((ready_size = select(FD_SETSIZE, &ready_fds, NULL, NULL, NULL)) < 0)
		{
				perror("select()");
				exit(EXIT_FAILURE);			
		}
		std::cout << ready_size << " sockets are ready!!\n";
		i = -1;
		while (++i < FD_SETSIZE)
		{
			if (FD_ISSET(i, &ready_fds))
			{
				if (isserver(i, servers))
				{
					 std::cout << "connection accepted..." << std::endl;

					if ((new_sockfd = accept(i, NULL, NULL)) < 0)
					{
						perror("accept()");
						exit(EXIT_FAILURE);
					}
					FD_SET(new_sockfd, &current_fds);
				}
				else
				{
					std::cout << "sending response..." << std::endl;
					send_response(i);
					std::cout << "response sent..." << std::endl;
					close(i);
					std::cout << "connection closed..." << std::endl;
					FD_CLR(i, &current_fds);
				}
			}
		}
	}
    return 0;
}

bool	isserver(int fd, std::vector<Server>& servers)
{
	std::vector<Server>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
		if (fd == it->sockfd)
			return true;
	return false;
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