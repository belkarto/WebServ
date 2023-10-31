#include "Server.hpp"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

void init_servers_pool(t_pool &serversPool);
void errAnExit(const char *er);
void readRequest(int socketFd);
void sendReponse(int socketFD);
void getRequestAndRespond(std::vector<t_dataPool> &data, fd_set &masterSet);
void initData(t_dataPool &data, int socketFd, int connectionFd);

int main() {
  t_pool serversPool;
  int connFd;
  std::vector<t_dataPool> data;
  t_dataPool dataSet;

  init_servers_pool(serversPool);
  while (1) {
    serversPool.workinSet = serversPool.masterSet;
    std::cout << "waiting on select....." << std::endl;

    if ((serversPool.ready_size =
             select(serversPool.max_socket, &serversPool.workinSet, NULL, NULL,
                    NULL)) < 0)
      errAnExit("select()");
    for (int i = 0; i < serversPool.max_socket && serversPool.ready_size > 0;
         i++) {

      if (FD_ISSET(i, &serversPool.workinSet)) {
        serversPool.ready_size -= 1;
        if ((connFd = accept(i, NULL, NULL)) < 0)
          errAnExit("accept()");
        initData(dataSet, i, connFd);
        data.push_back(dataSet);
        FD_SET(connFd, &serversPool.masterSet);
        if (connFd > serversPool.max_socket)
          serversPool.max_socket = connFd;
      }
      getRequestAndRespond(data, serversPool.masterSet);
      std::cout << "[[[[[[[--  client served  --]]]]]]]\n\n\n" << std::endl;
    }
  }
  return (EXIT_SUCCESS);
}

void initData(t_dataPool &data, int socketFd, int connectionFd) {
  data.isToRead = true;
  data.isToWrite = false;
  data.responsFd = -1;
  data.socketFd = socketFd;
  data.connectionFd = connectionFd;
}

void sendGetHeaders(int fd, size_t len) {
  std::stringstream ss;
  std::string sLen;
  write(fd, "HTTP/1.1 200 OK\n", 16);
  write(1, "HTTP/1.1 200 OK\n", 16);
  write(fd, "Content-Type: text/html\n", 24);
  write(1, "Content-Type: text/html\n", 24);
  write(fd, "Content-Length: ", 16);
  write(1, "Content-Length: ", 16);
  ss << len;
  ss >> sLen;
  write(fd, sLen.c_str(), sLen.size());
  write(1, sLen.c_str(), sLen.size());
  write(fd, "\n\n", 2);
  write(1, "\n\n", 2);
}

void getMethod(const char *filePath, int fd) {
  std::ifstream file(filePath);
  size_t fileLenght;
  std::stringstream ss;
  std::string str;

  if (file.is_open()) {
    file.seekg(0, std::ios::end);
    fileLenght = file.tellg();
    file.seekg(0, std::ios::beg);
    sendGetHeaders(fd, fileLenght);
    while (std::getline(file, str)) {
      write(fd, str.c_str(), str.length());
      write(fd, "\n", 1);
      std::cout << str.c_str() << std::endl;
    }
    write(fd, "\n\n\n", 3);
  }
}

void sendReponse(int socketFd) {
  // if get then read fetch the desired file
  // else if post upload the file
  // else if delet fetch the file and delet it
  // else its not a valid method
  getMethod("index.html", socketFd);
}

void readRequest(int socketFd) {
  char buffer[4096];
  ssize_t readed;

  std::cout << "getting request... from " << socketFd << std::endl;
  if ((readed = read(socketFd, buffer, 4096)) < 0)
    errAnExit("read()");
  std::cout << readed << " readed from request" << std::endl;
  write(1, buffer, readed);
}

void getRequestAndRespond(std::vector<t_dataPool> &data, fd_set &masterSet) {
  size_t i = 0;
  for (;!data.empty();) {
    if (data[i].isToRead)
    {
      readRequest(data[i].connectionFd);
      data[i].isToRead = false;
      data[i].isToWrite = true;
    }
    if (data[i].isToWrite)
    {
      sendReponse(data[i].connectionFd);
      data[i].isToWrite = false;
    }
    if (!data[i].isToRead && !data[i].isToWrite){
      close(data[i].connectionFd);
      FD_CLR(data[i].connectionFd, &masterSet);
      data.erase(data.begin() + i);
    }
    i++;
    if (i > data.size())
      i = 0;
  }
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
