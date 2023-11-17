#ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

#include "define.hpp"
#include "webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"


class Multiplexer
{
	SERVVECT					&servers;
	CLIENTVECT					clients;
	int							epfd;
	int							num_events;
	struct epoll_event			events[MAX_EVENTS];
	std::vector<std::string>    headers_fields;

	public:
		Multiplexer(SERVVECT	&servers);
		~Multiplexer();

		void		registerServers();
		void		registerClient(SERVIT& serverIt);
		void		dropClient(CLIENTIT& clientIt);
		void		connectionListener();
		void		getClientRequest(CLIENTIT& clientIt);
		void		parseRequestLine(CLIENTIT& clientIt);
		void		parseRequestHeaders(CLIENTIT& clientIt);
		void		reviewHeaders(CLIENTIT& clientIt);
		void		sendResponseHeaders(CLIENTIT& clientIt);
		void		sendResponse(CLIENTIT& clientIt);
		void		dropInactiveClients();
		void		loadMimeTypes();
		SERVIT		findListenSocket(int socket, SERVVECT &sockets);
		CLIENTIT	findConnectSocket(int socket, CLIENTVECT &sockets);

		static const char*				fields[HEADERS_FIELDS_SIZE];
		static void (Client::*fields_setters[HEADERS_FIELDS_SIZE])(std::string &field);
		static std::map<std::string, std::string>	mime_types;
    //------------ respons functions -------------------------------//
    void  sendResponseToClient(Client clientData);
    void  setErrorTemplate(CLIENTIT& , const std::string);
    SERVIT getMatchingServer(std::string &str, int socketFd);
    //------------------ error ------------------------------------// 
    void setErrTemp(Server &, Client &);
};
#endif
