#ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

#include "webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "ConfigParser.hpp"


void	send_response_body(Client &client);
void	send_response_headers(Client &client);

class Multiplexer
{
	std::vector<Server>				&servers;
	std::vector<Client>				clients;
	int								epfd;
	int								num_events;
	struct epoll_event				events[MAX_EVENTS];

	public:
		Multiplexer(std::vector<Server>	&servers);
		~Multiplexer();

		void							registerServers();
		void							registerClients(std::vector<Server>::iterator& serverIt);
		void							connectionListner();
		void							getClientRequest(std::vector<Client>::iterator& clientIt);
		void							sendResponseHeaders(std::vector<Client>::iterator& clientIt);
		void							sendResponse(std::vector<Client>::iterator& clientIt);
		std::vector<Server>::iterator	findListenSocket(int socket, std::vector<Server> &sockets);
		std::vector<Client>::iterator	findConnectSocket(int socket, std::vector<Client> &sockets);

};
#endif
