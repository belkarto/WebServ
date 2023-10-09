#include "WebServ.hpp"

int main(void)
{
	signal(SIGINT, sig_handler);
	Server	server;
	server.setup();
	while (true)
	{
		server.acceptConnections();
		while (!server.clients.empty())
		{
			std::cout << server.clients.back().getHeaders();
			// close(server.clients.back().sock.fd);
			server.clients.pop();
		}
		// int fd = open("example.html", O_RDONLY);
		// std::ifstream infile("example.html");
		// infile.seekg(0, std::ios::end);
		// size_t content_length = infile.tellg();
		// infile.seekg(0, std::ios::beg);
		// std::stringstream stream;
		// stream << content_length;
		// std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + stream.str() + "\r\n\r\n";
		// send(client_sockfd, response_header.c_str(), response_header.length(), 0);
		// size_t val;
		// while ((val = read(fd, buffer, CHUNK)) > 0)
		// 	send(client_sockfd, buffer, val, 0);			
		// close(client_sockfd);
	}
	return 0;
}

/*

*/