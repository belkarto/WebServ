#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <sstream>

#define CHUNK 1024

class Server {
public:
  const char *node;
  const char *service;
  struct addrinfo *bind_addr;
  int sockfd;

  Server(const char *node, const char *service);
  void setupServer(void);
};

typedef struct {
  int socket_fd;
  std::vector<Server> servers_vec;
  fd_set masterSet;
  fd_set workinSet;
  int max_socket;
  int ready_size;
} t_pool;

std::string get_headers(int sockfd);
#endif
