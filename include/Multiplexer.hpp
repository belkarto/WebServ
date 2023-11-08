#ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

#include "webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "ConfigParser.hpp"


void send_response(std::vector<Client>::iterator &clientIt);

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
		void							registerClient(std::vector<Server>::iterator& serverIt);
		int								dropClient(std::vector<Client>::iterator& clientIt);
		void							connectionListener();
		int								getClientRequest(std::vector<Client>::iterator& clientIt);
		int								getRequestHeaders(std::vector<Client>::iterator& clientIt);
		void							parseRequestLine(std::vector<Client>::iterator& clientIt);
		void							parseRequestHeaders(std::vector<Client>::iterator& clientIt);
		void							sendResponseHeaders(std::vector<Client>::iterator& clientIt);
		void							sendResponse(std::vector<Client>::iterator& clientIt);
		void							dropInactiveClients();
		std::vector<Server>::iterator	findListenSocket(int socket, std::vector<Server> &sockets);
		std::vector<Client>::iterator	findConnectSocket(int socket, std::vector<Client> &sockets);
};
#endif
