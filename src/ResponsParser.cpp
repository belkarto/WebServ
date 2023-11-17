#include "../include/Multiplexer.hpp"
#include <sstream>

void Multiplexer::setErrorTemplate(Client client,const std::string error)
{
  std::stringstream ss(error);
  client.error = true;
  ss >> client.errData.statuCode;
  std::cout << client.errData.statuCode << std::endl;
}

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
  if (!clientData.response_template_set) {
    SERVIT it = getMatchingServer(clientData.fields["host"], clientData.listen_socket);
    std::cout << *it->server_name.begin() << std::endl;
    std::cout << it->root << std::endl;
    std::cout << "requested server" << clientData.fields["host"] << std::endl;
    clientData.response_template_set = true;
    exit(1);
  } else {
    // start sending response
  }
}
