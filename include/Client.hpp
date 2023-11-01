#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

class Client
{
    public:
        int sockfd;
        int listen_socket;
        int read;
        int write;
};
#endif
