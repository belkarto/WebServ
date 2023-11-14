#ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

#include "webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"


class Multiplexer
{
	std::vector<Server>					&servers;
	std::vector<Client>					clients;
	int									epfd;
	int									num_events;
	struct epoll_event					events[MAX_EVENTS];
	std::vector<std::string>     		headers_fields;

	public:
		Multiplexer(std::vector<Server>	&servers);
		~Multiplexer();

		void							registerServers();
		void							registerClient(std::vector<Server>::iterator& serverIt);
		void							dropClient(std::vector<Client>::iterator& clientIt);
		void							connectionListener();
		void							getClientRequest(std::vector<Client>::iterator& clientIt);
		void							parseRequestLine(std::vector<Client>::iterator& clientIt);
		void							parseRequestHeaders(std::vector<Client>::iterator& clientIt);
		void							sendResponseHeaders(std::vector<Client>::iterator& clientIt);
		void							sendResponse(std::vector<Client>::iterator& clientIt);
		void							dropInactiveClients();
		std::vector<Server>::iterator	findListenSocket(int socket, std::vector<Server> &sockets);
		std::vector<Client>::iterator	findConnectSocket(int socket, std::vector<Client> &sockets);
    void              sendResponseToClient(Client clientData);

		static const char*				fields[HEADERS_FIELDS_SIZE];
		static void (Client::*fields_setters[HEADERS_FIELDS_SIZE])(std::string &field);
		static std::map<std::string, std::string>	mime_types;
};
#endif
