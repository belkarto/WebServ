#include "../include/Multiplexer.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
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

void setErrTemp(CLIENTIT &client) {
  client->ResTemplate.ResponsStatus = client->errData.errorHeader;
  client->ResTemplate.server = SERVER;
  client->ResTemplate.ContentEncoding = "chunked";
  client->ResTemplate.connenction = "close";
  client->ResTemplate.responsFilePath =
      getErrorFile(*client->serverIt, client->errData.statuCode);
}

static void sendingHeaders(CLIENTIT client) {
  send(client->connect_socket, PROTOCOL_VERS, 9, 0);
  send(client->connect_socket, client->ResTemplate.ResponsStatus.c_str(),
       client->ResTemplate.ResponsStatus.length(), 0);
  send(client->connect_socket, "\nServer: ", 9, 0);
  send(client->connect_socket, client->ResTemplate.server.c_str(),
       client->ResTemplate.server.length(), 0);
  send(client->connect_socket, "\nContent-Type: ", 15, 0);
  send(client->connect_socket, client->ResTemplate.contentType.c_str(),
       client->ResTemplate.contentType.length(), 0);
  send(client->connect_socket, "\nContent-Length: ", 17, 0);
  send(client->connect_socket, client->ResTemplate.contentLenght.c_str(),
       client->ResTemplate.contentLenght.length(), 0);
  send(client->connect_socket, "\r\n\r\n", 4, 0);
}

static void setTemplateTo404(CLIENTIT &client) {
  std::string path;
  client->ResTemplate.ResponsStatus.clear();
  client->ResTemplate.ResponsStatus = "404 Not found";
  client->ResTemplate.contentType = "text/html";
  client->ResTemplate.server = SERVER;
  path = getErrorFile(*client->serverIt, 404);
  if (!path.empty()) {
    std::ifstream file(path.c_str());
    std::stringstream ss;
    client->ResTemplate.responsFilePath = path;
    ss << getFileSize(&file);
    ss >> client->fileSize;
    client->ResTemplate.contentLenght = ss.str();
    file.close();
  } else {
    client->ResTemplate.responsFilePath.clear();
    client->ResTemplate.contentLenght = "194";
  }
}

std::string Multiplexer::getFileType(std::string fileName) {
  size_t pos;
  pos = fileName.find_last_of('.');
  if (pos == std::string::npos)
    return DEF_FILE_TYPE;
  std::map<std::string, std::string>::iterator it;
  it = this->mime_types.find(fileName.substr(pos));
  return it->second;
}

void Multiplexer::checkFilePath(CLIENTIT &client) {
  std::cout << "in check file = " << client->ResTemplate.responsFilePath << std::endl;
  if (client->ResTemplate.responsFilePath.empty() ||
      access(client->ResTemplate.responsFilePath.c_str(), R_OK) != 0)
  {
    std::cout << __func__ << std::endl;
    setTemplateTo404(client);
  }
  else {
    std::ifstream file(client->ResTemplate.responsFilePath.c_str());
    std::stringstream ss;
    ss << getFileSize(&file);
    ss >> client->fileSize;
    client->ResTemplate.contentLenght = ss.str();
    file.close();
    client->ResTemplate.contentType =
        getFileType(client->ResTemplate.responsFilePath);
  }
}

void Multiplexer::sendingRespons(CLIENTIT &client) {
  char buffer[BUFFER_SIZE];
  int  readed;

  if (!client->ResTemplate.headersSent) {
    checkFilePath(client);
    sendingHeaders(client);
    client->ResTemplate.headersSent = true;
    client->ResponseFile =
        new std::ifstream(client->ResTemplate.responsFilePath.c_str());
  }
  else 
  {
    if (client->fileSize == 0)
    {
      client->ResponseFile->close();
      client->response_all_sent = true;
      return;
    }
    client->ResponseFile->read(buffer, BUFFER_SIZE);
    readed = client->ResponseFile->gcount();
    send(client->connect_socket, buffer, readed, 0);
    client->fileSize -= readed;
  }
}

bool isDir(std::string &uri)
{
  std::string::iterator it;
  if (uri.find_last_of('/') != 0  && uri.find_last_of('/') == uri.length())
    return true;
  return false;
}

void getFilePath(CLIENTIT &client)
{
  std::cout << __func__ << " called" << std::endl;
  //TODO:
  //check rediriction and set the right path
  if (client->serverIt->location.empty())
    client->ResTemplate.responsFilePath = client->serverIt->root + client->fields[URI];
  else
  {
    std::vector<Location>::iterator it = client->serverIt->location.begin();
    for (;it != client->serverIt->location.end(); ++it)
    {
      if (client->fields[URI].compare(0, it->uri.length(), it->uri) == 0)
      {
        client->ResTemplate.responsFilePath = it->root + client->fields[URI];
        return;
      }
    }
  }
}

void Multiplexer::setResponseTemplate(CLIENTIT &client) {
    // if (isDir(client->fields[URI]))
    //   std::cout << "request directory" << std::endl;
    // else
    // {
      getFilePath(client);
      checkFilePath(client);
      client->ResTemplate.ResponsStatus = "200 OK";
    // }
}

void Multiplexer::sendResponseToClient(CLIENTIT &clientData) {
  static int visit = 0;
  if (!clientData->response_template_set) {
    clientData->serverIt = getMatchingServer(clientData->fields["host"],
                                             clientData->listen_socket);
    if (clientData->error)
      setErrTemp(clientData);
    else
      setResponseTemplate(clientData);
    std::cout << "after setting template "<< clientData->ResTemplate.responsFilePath << std::endl;
    clientData->response_template_set = true;
  } else {
    std::cout << visit++ <<" befor sending = " << clientData->ResTemplate.responsFilePath << std::endl;
    sendingRespons(clientData);
  }
}
