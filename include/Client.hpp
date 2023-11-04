#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

# define MAX_SIZE  1024
class Client
{
    public:
        int                 connect_socket;
        char*               str_addr;
        int                 listen_socket;
        char                buffer[MAX_SIZE];
        int                 recieved;
        int                 sent;
        std::ifstream       *response;
        int                 response_size;
        std::string         headers;
        bool                sending;
        bool                recieving;
        std::string         path;
        time_t              last_activity;

        Client();

        void    resetState();

   
};
#endif
