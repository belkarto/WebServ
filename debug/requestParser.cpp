#include "Server.hpp"
#include <sys/socket.h>

requestHeaders::requestHeaders(int connectionFd) {
  ssize_t readed;
  char buffer[BUFFER_SIZE];
  int headersState;
  std::string headers;

  seekIndex = 0;
  while (true) {
    // readed = recv(connectionFd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
    readed = read(connectionFd, buffer, BUFFER_SIZE);
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
      std::cout << headers << std::endl;
      exit(100);
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
  std::istringstream iss(buffer);
  std::string validMethods[] = {"GET", "POST", "DELETE"};
  int wordsInLine;
  int lenght = 0;

  while (std::getline(iss, line)) {
    wordsInLine = wordCount(line);
    std::cout << wordsInLine << std::endl;
    if (wordsInLine == 0)
      return HEADERS_END;
  }
  lenght = buffer.find_last_of("\n");
  buffer.erase(0, lenght + 1);
  return CONTINUE;
}

