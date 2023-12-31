#include "../include/Multiplexer.hpp"

char **Multiplexer::env;

std::map<std::string, std::string> Multiplexer::mime_types;

std::map<int, std::string> Multiplexer::defErrorPages;

int Multiplexer::connections = 0;

const char *Multiplexer::defErrorPagesStrings[NUM_DEF_ERROR] = {
    STATUS_100, STATUS_101, STATUS_200, STATUS_201, STATUS_202, STATUS_203, STATUS_204, STATUS_205, STATUS_206,
    STATUS_300, STATUS_301, STATUS_302, STATUS_303, STATUS_304, STATUS_305, STATUS_306, STATUS_307, STATUS_400,
    STATUS_401, STATUS_402, STATUS_403, STATUS_404, STATUS_405, STATUS_406, STATUS_407, STATUS_408, STATUS_409,
    STATUS_410, STATUS_411, STATUS_412, STATUS_413, STATUS_414, STATUS_415, STATUS_416, STATUS_417, STATUS_500,
    STATUS_501, STATUS_502, STATUS_503, STATUS_504, STATUS_505};

const char *Multiplexer::fields[HEADERS_FIELDS_SIZE] = {"host",       "content-type",      "content-length",
                                                        "connection", "transfer-encoding", "cookie"};

void (Client::*Multiplexer::fields_setters[HEADERS_FIELDS_SIZE])(std::string &field) = {
    &Client::setHost,       &Client::setContentType,      &Client::setContentLength,
    &Client::setConnection, &Client::setTransferEncoding, &Client::setCookie,
};

Multiplexer::Multiplexer(SERVVECT &servers, char **env) : servers(servers)
{
    this->env = env;
    epfd = epoll_create1(0);
    if (epfd < 0)
    {
        perror("epoll_create1()");
        exit(EXIT_FAILURE);
    }
    headers_fields.assign(fields, fields + HEADERS_FIELDS_SIZE);
    this->loadMimeTypes();
    this->loadDefErrorPages();
    this->registerServers();
    this->connectionListener();
}

Multiplexer::~Multiplexer()
{
    SERVIT serverIt;

    serverIt = servers.begin();
    for (; serverIt != servers.end(); serverIt++)
    {
        freeaddrinfo(serverIt->bind_addr);
        close(serverIt->listen_socket);
    }
}

void Multiplexer::registerServers()
{
    std::map<std::string, int>           sockfds;
    std::map<std::string, int>::iterator it2;
    SERVIT                               it;

    it = servers.begin();
    for (; it != servers.end(); it++)
    {
        if ((it2 = sockfds.find(it->bind_addr_str)) == sockfds.end())
        {
            it->initServer();
            sockfds[it->bind_addr_str] = it->listen_socket;
            epoll_add(epfd, it->listen_socket);
        }
        else
            it->listen_socket = it2->second;
    }
}

void Multiplexer::registerClient(SERVIT &serverIt)
{
    Client client;

    serverIt->acceptConnection(client);
    if (client.connect_socket < 0)
        return;
    client.serverIt = serverIt;
    epoll_add2(epfd, client.connect_socket);
    clients.push_back(client);
}

void Multiplexer::dropClient(CLIENTIT &clientIt)
{
    close(clientIt->connect_socket);
    clients.erase(clientIt);
}

void Multiplexer::connectionListener()
{
    int      i;
    SERVIT   serverIt;
    CLIENTIT clientIt;

    while (Running)
    {
        if ((num_events = epoll_wait(epfd, events, MAX_EVENTS, -1)) < 1)
            continue;
        i = -1;
        while (++i < num_events)
        {
            if ((serverIt = this->findListenSocket(events[i].data.fd, servers)) != servers.end())
                registerClient(serverIt);
            if ((clientIt = this->findConnectSocket(events[i].data.fd, clients)) != clients.end())
            {
                if ((events[i].events & EPOLLIN))
                {
                    // note it always enter here causing problems in post request
                    // i did quick fix by adding if statement in the beginning of the function
                    if (!getClientRequest(clientIt))
                        continue;
                }
                if (ConnectionTimedOut(clientIt))
                    dropClient(clientIt);
                else if (clientIt->response_all_sent)
                {
                    if (clientIt->response.connection == "close")
                        dropClient(clientIt);
                    else
                        clientIt->resetState();
                }
                else if ((events[i].events & EPOLLOUT))
                    handleResponse(clientIt);
            }
        }
    }
}

void Multiplexer::handleResponse(CLIENTIT &clientIt)
{
    if (clientIt->request_all_processed)
    {
        if (clientIt->response.cgi)
        {
            if (!clientIt->start_responding)
                clientIt->response.checkCgiTimeout(clientIt);
            else
                clientIt->response.sendResponseBuffer(clientIt);
        }
        else if (!clientIt->start_responding)
        {
            if (clientIt->fields["method"] == "GET")
                clientIt->response.setGetResponse(clientIt);
            else if (clientIt->fields["method"] == "POST")
                clientIt->response.setPostResponse(clientIt);
            else if (clientIt->fields["method"] == "DELETE")
                clientIt->response.setDeleteResponse(clientIt);
        }
        else
            clientIt->response.sendResponseBuffer(clientIt);
    }
}

SERVIT Multiplexer::findListenSocket(int socket, SERVVECT &sockets)
{
    SERVIT serverIt;

    serverIt = sockets.begin();
    while (serverIt != sockets.end() && serverIt->listen_socket != socket)
        serverIt++;
    return serverIt;
}

CLIENTIT Multiplexer::findConnectSocket(int socket, CLIENTVECT &sockets)
{
    CLIENTIT clientIt;

    clientIt = sockets.begin();
    while (clientIt != sockets.end() && clientIt->connect_socket != socket)
        clientIt++;
    return clientIt;
}

bool Multiplexer::getClientRequest(CLIENTIT &clientIt)
{
    ssize_t r;

    // if statement i did add
    if (clientIt->request_all_processed)
        return true;
    // end of if statement
    try
    {
        clientIt->header_buffer = new char[CLIENT_HEADER_BUFFER_SIZE + 1];
        r = recv(clientIt->connect_socket, clientIt->header_buffer, CLIENT_HEADER_BUFFER_SIZE, 0);
        clientIt->response.request_read = r;
        clientIt->header_buffer[r] = 0;
        if (r < 1)
        {
            delete[] clientIt->header_buffer;
            dropClient(clientIt);
            return false;
        }
        else
        {
            clientIt->headers.append(clientIt->header_buffer, r);
            if (!clientIt->request_line_received)
                parseRequestLine(clientIt);
            if (!clientIt->headers.empty())
                parseRequestHeaders(clientIt);
        }
    }
    catch (RequestParsingException &e)
    {
        clientIt->request_all_processed = true;
        clientIt->response.status = e.what();
        clientIt->response.setErrorResponse(clientIt);
    }
    if (clientIt->fields["method"] != "POST")
        delete[] clientIt->header_buffer;
    return true;
}

bool Multiplexer::ConnectionTimedOut(CLIENTIT &clientIt)
{
    if (KEEPALIVE_CONN && !clientIt->request_line_received)
    {
        time_t elapsed = time(NULL) - clientIt->last_activity;
        if (!clientIt->keepalive_requests)
        {
            if (elapsed >= CLIENT_HEADER_TIMEOUT)
                return true;
        }
        else
        {
            if (elapsed >= KEEPALIVE_TIMEOUT)
                return true;
        }
    }
    return false;
}

void Multiplexer::loadMimeTypes()
{
    std::string       line, key, value;
    std::stringstream ss;
    std::ifstream     infile(MIMETYPE_PATH);

    if (!infile.is_open())
        return;
    while (getline(infile, line))
    {
        ss << line;
        ss >> key;
        ss >> value;
        mime_types[key] = value;
        key.clear();
        value.clear();
        ss.clear();
        ss.str("");
    }
    infile.close();
}

void Multiplexer::loadDefErrorPages()
{
    std::stringstream ss;
    int               errPageIndex = 0;

    for (int i = 0; i < NUM_DEF_ERROR; i++)
    {
        ss.str(defErrorPagesStrings[i]);
        ss >> errPageIndex;
        this->defErrorPages[errPageIndex] = defErrorPagesStrings[i];
        ss.clear();
    }
}
