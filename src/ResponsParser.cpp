#include "../include/Multiplexer.hpp"
#include <algorithm>

SERVIT Multiplexer::getMatchingServer(std::string &serverName, int socketFd) {
  SERVIT it;
  bool firstOccurence = true;
  for (SERVIT iter = this->servers.begin(); iter != this->servers.end();
       ++iter) {
    if (socketFd == iter->listen_socket) {
      if (firstOccurence) {
        it = iter;
        firstOccurence = !firstOccurence;
      }
      if (std::find(iter->server_name.begin(), iter->server_name.end(),
                    serverName) != iter->server_name.end())
        return iter;
    }
  }
  return it;
}

void Multiplexer::sendResponseToClient(Client clientData) {
  (void)clientData;
  SERVIT it =
      getMatchingServer(clientData.fields["host"], clientData.listen_socket);
  std::cout << *it->server_name.begin() << std::endl;
  std::cout << it->root << std::endl;
  std::cout << "requested server" << clientData.fields["host"] << std::endl;
  exit(1);
}
