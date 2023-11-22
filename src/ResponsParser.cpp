#include "../include/Multiplexer.hpp"

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
  client->ResTemplate.ContentEncoding = "chunked";
  client->ResTemplate.connenction = "close";
  client->ResTemplate.responsFilePath =
      getErrorFile(*client->serverIt, client->errData.statuCode);
}

static void sendingHeaders(CLIENTIT client) {
  int hearesLength = 5;
  std::string headersArr[] = {
      PROTOCOL_VERS,
      "Server: ",
      "Content-Type: ",
     "Transfer-Encoding: ",
      "Content-Length: ",
  };
  std::string tmp, responsHolder[]= {
    client->ResTemplate.ResponsStatus,
    SERVER,
    client->ResTemplate.contentType,
    client->ResTemplate.transferEncoding,
    client->ResTemplate.contentLenght
  };
  for (int i =0; i < hearesLength; i++)
  {
    if (responsHolder[i].empty())
      continue;
    tmp = headersArr[i] + responsHolder[i] + "\n";
    send(client->connect_socket, tmp.c_str(), tmp.length(), 0);
  }
  send(client->connect_socket, "\r\n", 2, 0);
}

static void setTemplateTo404(CLIENTIT &client) {
  std::string path;
  client->ResTemplate.ResponsStatus.clear();
  client->ResTemplate.ResponsStatus = "404 Not found";
  client->ResTemplate.contentType = "text/html";
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

bool isDir(std::string &uri) {
  std::string::iterator it;
  if (uri.find_last_of('/') != 0 && uri.find_last_of('/') == uri.length() - 1)
    return true;
  std::cout << uri.find_last_of('/') << " " << uri.length() << std::endl;
  return false;
}

void setDirectoryTemplat(ResponseTemplate &templat)
{
  templat.ResponsStatus = STATUS_200;
  templat.contentType = "text/html";
  templat.transferEncoding = "chunked";
  templat.dirReasHeaderSent = false;
}

void Multiplexer::checkFilePath(CLIENTIT &client) {
  if (client->ResTemplate.responsFilePath.empty() ||
      access(client->ResTemplate.responsFilePath.c_str(), F_OK) != 0) {
    setTemplateTo404(client);
  } else {
    if (isDir(client->ResTemplate.responsFilePath)) {
      setDirectoryTemplat(client->ResTemplate);
      client->ResTemplate.dirResponse = true;
      return;
    }
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

void sendingDirectoryIndex(CLIENTIT &client)
{
  if (!client->ResTemplate.dirReasHeaderSent)
    client->ResTemplate.sendTopHead(client->connect_socket);
  else
  {
    client->ResTemplate.sendDirBody(client->connect_socket);
    client->response_all_sent = true;
  }
}

void Multiplexer::sendingRespons(CLIENTIT &client) {
  char buffer[BUFFER_SIZE];
  int readed;

  if (!client->ResTemplate.headersSent) 
  {
    checkFilePath(client);
    sendingHeaders(client);
    client->ResTemplate.headersSent = true;
    client->ResponseFile =
        new std::ifstream(client->ResTemplate.responsFilePath.c_str());
  }
  else 
  {
    if (client->ResTemplate.dirResponse)
      sendingDirectoryIndex(client);
    else
    {
      if (client->fileSize == 0) 
      {
        client->ResponseFile->close();
        delete client->ResponseFile;
        client->ResponseFile = NULL;
        client->response_all_sent = true;
        return;
      }
      client->ResponseFile->read(buffer, BUFFER_SIZE);
      readed = client->ResponseFile->gcount();
      send(client->connect_socket, buffer, readed, 0);
      client->fileSize -= readed;
    }
  }
}

/* get file path handles rediriction and check if the path exist and have Read
 * permission */
void getFilePath(CLIENTIT &client) {
  client->ResTemplate.responsFilePath =
      client->serverIt->root + client->fields[URI];
  if (!client->serverIt->location.empty()) {
    std::vector<Location>::iterator it = client->serverIt->location.begin();
    for (; it != client->serverIt->location.end(); ++it) {
      if (client->fields[URI].compare(0, it->uri.length(), it->uri) == 0) {
        client->ResTemplate.responsFilePath = it->root + client->fields[URI];
        return;
      }
    }
  }
  if (client->fields[URI] == "/") {
    std::string tmp;
    for (STRINGVECT::iterator it = client->serverIt->index.begin();
         it != client->serverIt->index.end(); ++it) {
      tmp = client->ResTemplate.responsFilePath + *it;
      if (access(tmp.c_str(), F_OK | R_OK) == 0) {
        client->ResTemplate.responsFilePath = tmp;
        return;
      }
    }
  }
}

void Multiplexer::setResponseTemplate(CLIENTIT &client) {
  getFilePath(client);
  checkFilePath(client);
  client->ResTemplate.ResponsStatus = STATUS_200;
}

void Multiplexer::sendResponseToClient(CLIENTIT &clientData) {
  static int tracker = 0;
  if (!clientData->response_template_set) {
    clientData->serverIt = getMatchingServer(clientData->fields["host"],
                                             clientData->listen_socket);
    if (clientData->error)
      setErrTemp(clientData);
    else
      setResponseTemplate(clientData);
    clientData->response_template_set = true;
    std::cout << "request number " << ++tracker << std::endl;
    std::cout << clientData->ResTemplate.responsFilePath << std::endl;
  } else {
    sendingRespons(clientData);
  }
}
