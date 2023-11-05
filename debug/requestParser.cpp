#include "Server.hpp"
#include <sys/socket.h>

requestHeaders::requestHeaders(int connectionFd) {
  ssize_t readed;
  char buffer[BUFFER_SIZE];
  int headersState;
  std::string headers;

  seekIndex = 0;
  while (true) {
    readed = recv(connectionFd, buffer, BUFFER_SIZE, 0);
    // readed = read(connectionFd, buffer, BUFFER_SIZE);
    if (readed < 0) {
      perror("recv fialed");
      exit(1);
    } else if (readed == 0) {
      std::cerr << "connection closed" << std::endl;
    }
    std::string bufferInstens(buffer);
    headers.append(bufferInstens);
    headersState = headersParser(headers);
    if (headersState == HEADERS_END) {
      lseek(connectionFd, seekIndex, SEEK_SET);
      break;
    }
  }
}

int wordCount(std::string str) {
  int count = 0;
  std::stringstream ss(str);
  std::string word;
  while (ss >> word)
    count++;
  return (count);
}

int requestHeaders::headersParser(std::string &buffer) {
  if (buffer.find("\n") == std::string::npos)
    return CONTINUE;
  std::string line;
  int wordsInLine;
  size_t pos;
  std::string requestHeaders::*attribs[] = {
      &requestHeaders::contentType,
      &requestHeaders::Host,
      &requestHeaders::connection,
      &requestHeaders::contentLenght,
  };
  std::string directiveList[] = {
      "Content-Type:", "Host:", "Connection:", "Content-Length:"};

  while (true) {
    pos = buffer.find("\n");
    if (pos == std::string::npos)
      return CONTINUE;
    line = buffer.substr(0, pos);
    seekIndex += line.size() + 1;
    buffer.erase(0, pos + 1);
    wordsInLine = wordCount(line);
    if (!wordsInLine)
      return HEADERS_END;
    std::istringstream iss(line);
    if (wordsInLine == 3) {
      iss >> methodType >> URI >> ProtocolVersion;
    } else {
      std::string directive;
      iss >> directive;
      for (int i = 0; i < DIRECT_LIST_SIZE; i++) {
        if (directive == directiveList[i])
          iss >> this->*attribs[i];
      }
    }
  }
}
