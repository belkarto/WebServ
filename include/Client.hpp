#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "WebServ.hpp"

class Client
{
    public:
        Socket sock;
        std::string getHeaders();
};
#endif