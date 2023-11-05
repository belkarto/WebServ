#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"


class Client
{
    public:
        int                 connect_socket;
        int                 listen_socket;
        char*               header_buffer;
        std::string         headers;
        time_t              header_timeout;
        bool                headers_recieved;
        bool                request_line_received;
        time_t              last_activity;
        std::string         method;
        std::string         request_uri;

        Client();

        void    resetState();
        bool    headerTimedout();
};
#endif
