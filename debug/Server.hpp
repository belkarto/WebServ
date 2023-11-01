#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define CHUNK 1024
# define PACKET_SIZE 2024

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

typedef struct {
  int socketFd;
  int connectionFd;
  int responsFile;
  size_t responsFileLenght;
  size_t writedRespons;
  bool sentHeaders;
  bool isToRead;
  bool isToWrite;
  bool connectionClosed;
} t_dataPool;


// class t_dataPool {
//   std::ifstream *responsFile;
// public:
//   int socketFd;
//   int connectionFd;
//   bool sentHeaders;
//   bool isToRead;
//   bool isToWrite;
// };

std::string get_headers(int sockfd);
#endif
