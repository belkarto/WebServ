#include "Server.hpp"

void    Server::setup()
{
    int reuse;

    sock.addrlen = sizeof(sock.addr);
    /* Creating the socket */
	if ((sock.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket creation failed");
        exit(EXIT_FAILURE);
	}
    /* bind failed: Address already in use */
    reuse = 1;
	if (setsockopt(sock.fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt failed");
        exit(EXIT_FAILURE);
	}
    /* Configuration socket */
    sock.addr.sin_family = AF_INET;
    sock.addr.sin_port = htons(224);
    sock.addr.sin_addr.s_addr = INADDR_ANY;
	memset(sock.addr.sin_zero, 0, sizeof(sock.addr.sin_zero));
	if (bind(sock.fd, (struct sockaddr *) &sock.addr, sock.addrlen) < 0)
	{
		perror("bind failed");
        exit(EXIT_FAILURE);
	}
    /* setting socket in listen mode*/
    if (listen(sock.fd, 0) < 0)
	{
		perror("listen failed");
        exit(EXIT_FAILURE);
	}
}

void    Server::acceptConnections(void)
{
    Client client;

    client.sock.addrlen = sizeof(client.sock.addr);
    if ((client.sock.fd = accept(sock.fd, (sockaddr *) &client.sock.addr, &client.sock.addrlen)) < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
	clients.push(client);
}

// void    Server::sendResponse(Client &client)
// {

// }