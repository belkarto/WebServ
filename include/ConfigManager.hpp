#ifndef CONFIGMANAGER_HPP
# define CONFIGMANAGER_HPP

#include "webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "ConfigParser.hpp"

class ConfigManager
{
	std::vector<Server>	servers;
	std::vector<Client>	clients;
	int					epfd;		// epoll instance		

	public:
		ConfigManager(const char *config_path);
		~ConfigManager();

		void							createEpollInstance();
		void							fillEpollSet();
		void							eventListener();
		std::vector<Server>::iterator	findListenSocket(int sockfd);
		std::vector<Client>::iterator	findConnectSocket(int sockfd);
};
#endif
