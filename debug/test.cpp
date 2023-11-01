#include "Server.hpp"

bool isserver(int fd, std::vector<Server> &servers);

int main(void)
{
	int new_sockfd;
	std::vector<Server> servers;
	fd_set current_fds, read_fds;
	int i;
	int max_socket = 0;

	servers.emplace_back("localhost", "1026");
	servers.emplace_back("127.0.0.1", "1025");
	// servers.emplace_back("127.0.0.2", "1025");
	// servers.emplace_back("127.0.0.3", "1025");

	FD_ZERO(&current_fds);
	FD_SET(servers[0].sockfd, &current_fds);
	FD_SET(servers[1].sockfd, &current_fds);
	FD_SET(servers[2].sockfd, &current_fds);
	FD_SET(servers[3].sockfd, &current_fds);
	max_socket = servers[3].sockfd;

	while (true)
	{
		// creat a copy of current_fds because select take fd_set by refrence and it modifies it
		// and it delets all fd_set that aren't ready for read
		read_fds = current_fds;
		int ready_size;
		if ((ready_size = select(max_socket, &read_fds, NULL, NULL, NULL)) < 0)
		{
			perror("select()");
			exit(EXIT_FAILURE);
		}
		std::cout << ready_size << " sockets are ready!!\n";
		std::cout << max_socket << " max_socket!!\n";
		for (int i = 0; i <= max_socket; i++)
		{
			if (FD_ISSET(i, &read_fds))
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
					if (new_sockfd > max_socket)
						max_socket = new_sockfd;
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

bool isserver(int fd, std::vector<Server> &servers)
{
	std::vector<Server>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
		if (fd == it->sockfd)
			return true;
	return false;
}

std::string get_headers(int sockfd)
{
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
