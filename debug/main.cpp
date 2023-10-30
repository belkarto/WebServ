#include "Server.hpp"
#include <cstdio>
#include <cstdlib>

void init_servers_pool(t_pool &serversPool);
void errAnExit(const char *er);
void readRequest(int socketFd);
void sendReponse(int socketFD);
void getRequestAndRespond(int socketFd, t_pool serversPool,
                          int connectedSocket);

int main() {
  t_pool serversPool;
  int connFd;
  init_servers_pool(serversPool);
  while (1) {
    serversPool.workinSet = serversPool.masterSet;
    std::cout << "waiting on select....." << std::endl;

    if ((serversPool.ready_size =
             select(serversPool.max_socket, &serversPool.workinSet, NULL, NULL,
                    NULL)) < 0)
      errAnExit("select()");

    std::cout << "ready sockets -------> " << serversPool.ready_size
              << std::endl;
    for (int i = 0; i < serversPool.max_socket && serversPool.ready_size > 0;
         i++) {
      if (FD_ISSET(i, &serversPool.workinSet)) {
        serversPool.ready_size -= 1;
        if ((connFd = accept(i, NULL, NULL)) < 0)
          errAnExit("accept()");
        std::cout << connFd << " " << i << std::endl;
        FD_SET(connFd, &serversPool.masterSet);
        if (connFd > serversPool.max_socket)
          serversPool.max_socket = connFd;
        getRequestAndRespond(i, serversPool, connFd);
        std::cout << "[[[[[[[--  client served  --]]]]]]]\n\n\n" << std::endl;
      }
    }
  }
  return (EXIT_SUCCESS);
}

void  sendReponse(int socketFd)
{

}

void readRequest(int socketFd) {
  char buffer[4096];
  size_t readed;

  std::cout << "getting request... from " << socketFd << std::endl;
  if ((readed = read(socketFd, buffer, 4096)) < 0)
    errAnExit("read()");
  std::cout << readed << " readed from request" << std::endl;
  write(1, buffer, readed);
}

void getRequestAndRespond(int socketFd, t_pool serversPool,
                          int connectedSocket) {
  std::cout << connectedSocket << std::endl;
  readRequest(connectedSocket);
  sendReponse(connectedSocket);
  close(connectedSocket);
  FD_CLR(connectedSocket, &serversPool.masterSet);
}

void errAnExit(const char *er) {
  perror(er);
  exit(EXIT_FAILURE);
}

void init_servers_pool(t_pool &serversPool) {
  size_t i;
  serversPool.servers_vec.emplace_back("127.0.0.1", "3433");
  serversPool.servers_vec.emplace_back("127.0.0.1", "3434");
  serversPool.servers_vec.emplace_back("127.0.0.1", "3435");
  serversPool.servers_vec.emplace_back("127.0.0.1", "3436");

  FD_ZERO(&serversPool.masterSet);
  for (i = 0; i < serversPool.servers_vec.size(); i++)
    FD_SET(serversPool.servers_vec[i].sockfd, &serversPool.masterSet);
  serversPool.max_socket = serversPool.servers_vec[i - 1].sockfd + 1;
}
