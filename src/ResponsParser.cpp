#include "../include/Multiplexer.hpp"
#include <algorithm>
#include <sstream>
#include <unistd.h>

void Multiplexer::setErrorTemplate(CLIENTIT &client, const std::string error) {
  std::stringstream ss(error);
  client->errData.errorHeader = error;
  client->error = true;
  ss >> client->errData.statuCode;
  client->request_all_processed = true;
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

static std::string getErrorFile(Server &server, int errorPageCode) {
  std::string path, defaultPath;
  std::stringstream ss;
  std::map<std::vector<int>, std::string>::iterator it =
      server.error_page.begin();
  for (; it != server.error_page.end(); ++it) {
    if (std::find(it->first.begin(), it->first.end(), errorPageCode) !=
        it->first.end()) {
      path = it->second;
      break;
    }
  }
  if (access(path.c_str(), R_OK) != 0) {
    path.clear();
    ss << errorPageCode;
    defaultPath = DEF_ERR_PATH + ss.str() + ".html";
    if (access(defaultPath.c_str(), R_OK) == 0)
      path = defaultPath;
  }
  return path;
}

void Multiplexer::setErrTemp(Server &server, CLIENTIT &client) {
  client->ResTemplate.server = SERVER;
  client->ResTemplate.ContentEncoding = "chunked";
  client->ResTemplate.connenction = "close";
  client->ResTemplate.responsFilePath =
      getErrorFile(server, client->errData.statuCode);
}

static void sendingHeaders(CLIENTIT client)
{
  send(client->connect_socket, client->ResTemplate.ResponsStatus.c_str(), client->ResTemplate.ResponsStatus.length(), 0);
}

static void sendingResponse(CLIENTIT &client)
{
  if (!client->ResTemplate.headersSent)
  {
    sendingHeaders(client);
    client->ResTemplate.headersSent = true;
  }
  else {
  
  }
}

void Multiplexer::sendResponseToClient(CLIENTIT &clientData) {
  if (!clientData->response_template_set) {
    SERVIT it = getMatchingServer(clientData->fields["host"],
                                  clientData->listen_socket);
    if (clientData->error)
      setErrTemp(*it, clientData);

    clientData->response_template_set = true;
    std::cout << clientData->error << " " << clientData->ResTemplate.server
              << " " << clientData->ResTemplate.ContentEncoding << " ->  " << clientData->ResTemplate.responsFilePath << std::endl;
  } else {
    sendingResponse(clientData);
    // start sending response
  }
}
