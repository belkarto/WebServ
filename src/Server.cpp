#include "Server.hpp"

Server::Server()
{
    addr_resolver(&bind_addr, DEFAULT_HOST, DEFAULT_PORT);
    index.push_back(DEFAULT_INDEX);
    root = DEFAULT_ROOT;
    client_max_body_size = DEFAULT_CLIENT_MAX_BODY_SIZE;
    autoindex = DEFAULT_AUTOINDEX;
}
