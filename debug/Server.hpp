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
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define CHUNK 1024
#define PACKET_SIZE 2024

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

class requestHeaders {
  std::string methodType;
  std::string URI;
  std::string ProtocolVersion;
  size_t contentLenght;
  std::string Host;
};

class responsHeaders {
  std::string Status; // Ex: 200 OK // if errno of open ==> ENOENT not found | EACCES permission denied
  std::string contentType;
  std::string contentLenght;
  std::string server;
  std::string ContentEncoding; // always will be set to chunked
};

std::string get_headers(int sockfd);
#endif
