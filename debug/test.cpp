# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <unistd.h>
# include <signal.h>
# include <arpa/inet.h>
# include <string.h>
# include <vector>
# include <fcntl.h>
# include <fstream>
# include <sstream>

# define CHUNK 1024

void	sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << "Process Terminated" << std::endl;
		exit(143);
	}
}

int main(void)
{
    int 				sockfd;
	int 				client_sockfd;
	struct sockaddr_in	sock_addr;
	struct sockaddr_in	client_sock_addr;
	socklen_t			client_socklen;	
	char                buffer[1024] = {0};
	ssize_t				readval;
	int					reuse;
	char sender_ip[INET_ADDRSTRLEN];

	/* Creating the socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket creation failed");
        exit(EXIT_FAILURE);
	}
	/* Fixing bind failed: Address already in use */
	reuse = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt failed");
        exit(EXIT_FAILURE);
	}
	/* Naming the socket */
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(224);
    sock_addr.sin_addr.s_addr = INADDR_ANY;
	// sock_addr.sin_addr.s_addr = inet_addr("10.30.239.200");
	memset(sock_addr.sin_zero, 0, sizeof(sock_addr.sin_zero));
	if (bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("bind failed");
        exit(EXIT_FAILURE);
	}
    /* listen to incoming connections */
	if (listen(sockfd, 0) < 0)
	{
		perror("listen failed");
        exit(EXIT_FAILURE);
	}
	signal(SIGINT, sig_handler);
	/* Accepting Connections*/
	while (true)
	{
		client_socklen = sizeof(client_sock_addr);
		if ((client_sockfd = accept(sockfd, (sockaddr *)  &client_sock_addr, &client_socklen)) < 0)
		{
			perror("accept failed");
            exit(EXIT_FAILURE);
		}
    	inet_ntop(AF_INET, &(client_sock_addr.sin_addr), sender_ip, INET_ADDRSTRLEN);
		std::cout << "Received request from: " << sender_ip << std::endl;
		std::string request_header = "";
		while ((readval = read(client_sockfd, buffer, 1024)) > 0)
		{
			request_header += std::string(buffer, readval);
			if (request_header.find_last_of("\r\n\r\n") != std::string::npos)
				break;
		}
		std::cout << request_header << std::endl;
		int fd = open("clapping_cat.gif", O_RDONLY);
		std::ifstream infile("clapping_cat.gif");
		infile.seekg(0, std::ios::end);
		size_t content_length = infile.tellg();
		infile.seekg(0, std::ios::beg);
		std::stringstream stream;
		stream << content_length;
		std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\nContent-Length: " + stream.str() + "\r\n\r\n";
		send(client_sockfd, response_header.c_str(), response_header.length(), 0);
		size_t val;
		while ((val = read(fd, buffer, CHUNK)) > 0)
			send(client_sockfd, buffer, val, 0);			
		close(client_sockfd);
	}
	close(sockfd);
	return 0;

}