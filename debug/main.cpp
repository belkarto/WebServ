#include "Server.hpp"
#include <cstdio>
#include <cstdlib>

void errAnExit(const char *er) {
  perror(er);
  exit(EXIT_FAILURE);
}

void init_servers_pool(t_pool &serversPool) {
  serversPool.servers_vec.emplace_back("localhost", "1026");
  serversPool.servers_vec.emplace_back("127.0.0.1", "1025");
  serversPool.servers_vec.emplace_back("127.0.0.2", "1025");
  serversPool.servers_vec.emplace_back("127.0.0.3", "1025");

  FD_ZERO(&serversPool.currentFdSet);
  FD_SET(serversPool.servers_vec[0].sockfd, &serversPool.currentFdSet);
  FD_SET(serversPool.servers_vec[1].sockfd, &serversPool.currentFdSet);
  FD_SET(serversPool.servers_vec[2].sockfd, &serversPool.currentFdSet);
  FD_SET(serversPool.servers_vec[3].sockfd, &serversPool.currentFdSet);
  serversPool.max_socket = serversPool.servers_vec[3].sockfd;
}

int main() {
  t_pool serversPool;

  init_servers_pool(serversPool);
  while (1) {
    serversPool.tmpFdSet = serversPool.currentFdSet;
    if ((serversPool.ready_size =
             select(serversPool.max_socket, &serversPool.tmpFdSet, NULL, NULL,
                    NULL)) < 0)
      errAnExit("select()");
    std::cout << "ready sockets " << serversPool.ready_size << std::endl;
    for (int i = 0 ; i < serversPool.max_socket; i++) {
      if (FD_ISSET(i, &serversPool.tmpFdSet))
      {

      }
    }
  }
  return (EXIT_SUCCESS);
}
