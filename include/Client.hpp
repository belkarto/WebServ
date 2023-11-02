#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

# define MAX_SIZE  1024
class Client
{
    public:
        int             sockfd;
        int             listen_socket;
        char            buffer[MAX_SIZE];
        int             recieved;
        int             sent;
        std::ifstream   *response;
        int             response_size;
        std::string     headers;
        bool            sending;
        std::string     path;

        Client(){}
        Client(int sockfd, int listen_socket);

        void    resetState();
};
#endif
