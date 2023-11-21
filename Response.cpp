#include "Response.hpp"

void Multiplexer::setErrorTemplate(CLIENTIT &client, const std::string error)
{
  std::stringstream ss(error);
  client->error = true;
  ss >> client->errData.statuCode;
  client->request_all_processed = true;
}

SERVIT Multiplexer::getMatchingServer(std::string &serverName, int socketFd)
{
  SERVIT it;
  bool firstOccurence = true;
  for (SERVIT iter = this->servers.begin(); iter != this->servers.end();
       ++iter)
  {
    if (socketFd == iter->listen_socket)
    {
      if (firstOccurence)
      {
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

void Multiplexer::setErrTemp(Server &server, Client &client)
{
  (void)server;
  client.ResTemplate.server = SERVER;
  client.ResTemplate.ContentEncoding = "chunked";
}

void Multiplexer::sendResponseToClient(Client clientData)
{
  if (!clientData.response_template_set)
  {
    SERVIT it = getMatchingServer(clientData.fields["host"], clientData.listen_socket);
    if (clientData.error)
      setErrTemp(*it, clientData);
    clientData.response_template_set = true;
    exit(1);
  }
  else
  {
    // start sending response
  }
}
