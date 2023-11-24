#ifndef SERVER_HPP
#define SERVER_HPP

#include "define.hpp"
#include "Client.hpp"
#include "webserv.hpp"

typedef struct Location
{
    std::string                         uri;
    std::string                         root;
    STRINGVECT                          index;
    STRINGVECT                          method;
    std::string                         redirect;
    bool                                autoindex;
    std::pair<std::string, std::string> cgi;
    std::string                         upload_store;

} Location;

class Server
{
  public:
    int                                     listen_socket;
    std::string                             bind_addr_str;
    struct addrinfo                        *bind_addr;
    STRINGVECT                              server_name;
    STRINGVECT                              index;
    std::string                             root;
    long long                               client_max_body_size;
    std::map<std::vector<int>, std::string> error_page;
    bool                                    autoindex;
    std::vector<Location>                   location;

    Server();
    void                                              emplaceBackLocation();
    void                                              initServer();
    void                                              acceptConnection(Client &client);
    void                                              findLocation(CLIENTIT &clientIt, std::string &uri);
    std::map<std::vector<int>, std::string>::iterator findErrorPage(int code);
};
#endif
