#ifndef SERVER_HPP
# define SERVER_HPP

# include "WebServ.hpp"

class Server
{
    public:
        Socket sock;
        std::queue<Client> clients;

        void    setup(void);
        void    acceptConnections(void);
        // void    sendResponse(Client &client);
};
#endif
