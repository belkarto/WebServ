#include "Server.hpp"
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

void init_servers_pool(t_pool &serversPool);
void errAnExit(const char *er);
void readRequest(t_dataPool &data);
void sendReponse(t_dataPool &data);
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
    for (size_t i = 0; i < data.size(); i++) {
      FD_SET(data[i].connectionFd, &serversPool.masterSet);
    }
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
        std::cout << serversPool.max_socket
                  << "---------------------------------------------"
                  << std::endl;
        initData(dataSet, i, connFd);
        data.push_back(dataSet);
        FD_SET(connFd, &serversPool.masterSet);
        if (connFd > serversPool.max_socket)
          serversPool.max_socket = connFd;
      }
      getRequestAndRespond(data, serversPool.masterSet);
    }
  }
  return (EXIT_SUCCESS);
}

void initData(t_dataPool &data, int socketFd, int connectionFd) {
  data.isToRead = true;
  data.isToWrite = false;
  // data.responsFile = -1;
  data.socketFd = socketFd;
  data.connectionClosed = false;
  data.connectionFd = connectionFd;
}

void sendGetHeaders(int fd, size_t len) {
  std::stringstream ss;
  std::string sLen;
  write(fd, "HTTP/1.1 200 OK\n", 16);
  write(fd, "Content-Type: text/html\n", 24);
  write(fd, "Content-Length: ", 16);
  ss << len;
  ss >> sLen;
  write(fd, sLen.c_str(), sLen.size());
  write(fd, "\n\n", 2);
}

void getMethod(const char *filePath, t_dataPool &data) {
  if ((data.responsFile = open(filePath + 1, O_RDONLY)) < 0)
    exit(1);

  std::ifstream respons(filePath);
  respons.seekg(0, std::ios::end);
  data.responsFileLenght = respons.tellg();
  data.writedRespons = 0;
  respons.seekg(0, std::ios::beg);
  sendGetHeaders(data.connectionFd, data.responsFileLenght);
  respons.close();
  data.sentHeaders = !data.sentHeaders;
}

void sendReponse(t_dataPool &data) {
  char buffer[PACKET_SIZE];
  int readed;
  std::cout << data.request->URI << std::endl;
  if (!data.sentHeaders)
    getMethod(data.request->URI.c_str(), data);
  else {
    readed = read(data.responsFile, buffer, PACKET_SIZE);
    write(data.connectionFd, buffer, readed);
    data.writedRespons += readed;
  }
  if (data.responsFileLenght == data.writedRespons) {
    data.isToWrite = false;
    write(data.connectionFd, "\n\n\n\n", 3);
  }
}

void readRequest(t_dataPool &data) {
  char buffer[PACKET_SIZE];
  ssize_t readed;

  std::cout << "getting request... from " << data.socketFd << std::endl;
  if ((readed = read(data.connectionFd, buffer, PACKET_SIZE)) < 0)
    errAnExit("read()");
  std::cout << readed << " readed from request" << std::endl;
  write(1, buffer, readed);
  if (readed == 0)
    data.isToRead = false;
  else if (readed < 0) {
    data.isToRead = false;
    close(data.connectionFd);
  } else if (strstr(buffer, "\r\n\r\n") != NULL) {
    // got the end of request
    data.isToRead = false;
    data.isToWrite = true;
    data.sentHeaders = false;
    data.writedRespons = 0;
  }
}

void getRequestAndRespond(std::vector<t_dataPool> &data, fd_set &masterSet) {
  size_t i = 0;
  for (; i < data.size(); i++) {
    if (data[i].isToRead) {
      requestHeaders readed(data[i]);
      data[i].request = &readed;
    }
    if (data[i].isToWrite) {
      std::cout << "sending Respons" << std::endl;
      sendReponse(data[i]);
    }
    if (!data[i].isToRead && !data[i].isToWrite) {
      if (!data[i].connectionClosed) {
        close(data[i].connectionFd);
        FD_CLR(data[i].connectionFd, &masterSet);
        data.erase(data.begin() + i);
      } else
        data[i].isToRead = true;
      close(data[i].responsFile);
      std::cout << "[[[[[[[--  client served  --]]]]]]]\n\n\n" << std::endl;
    }
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
