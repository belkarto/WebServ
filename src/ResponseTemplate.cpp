#include "../include/ResponseTemplate.hpp"
#include "../include/Multiplexer.hpp"
#include <sstream>
#include <sys/socket.h>

/* DEFAULT ERROR PAGES */
std::string ResponseTemplate::getErrorPage(int errorCode) {
  std::string finalResponse;
  std::map<int, std::string>::iterator it;

  finalResponse = "<html>" CRLF "<head><title>";
  it = this->errorPages.find(errorCode);
  finalResponse.append(it->second)
      .append("</title></head>" CRLF "<body>" CRLF "<center><h1>")
      .append(it->second)
      .append("</h1></center>" CRLF);

  return finalResponse;
}

ResponseTemplate::ResponseTemplate() {
  headersSent = false;
  dirResponse = false;
}

void ResponseTemplate::reset() {
  responsFilePath.clear();
  ResponsStatus.clear();
  ContentEncoding.clear();
  connenction.clear();
  contentType.clear();
  transferEncoding.clear();
  contentLenght.clear();
  responsFilePath.clear();
  dirReasHeaderSent = false;
  headersSent = false;
  dirResponse = false;
}

void ResponseTemplate::sendTopHead(int connectionFd) {
  std::string requestedIndex = "<html>\n<head>\n<title>Index of " +
                               this->responsFilePath +
                               "</title>\n</head>\n<body>\n<h1>Index of " +
                               this->responsFilePath + "</h1>\n<hr>\n<pre>\r\n";
  std::stringstream ss;
  std::string len;

  ss << requestedIndex.length();
  ss >> len;
  len.append("\r\n");
  send(connectionFd, len.c_str(), len.length(), 0);
  send(connectionFd, requestedIndex.c_str(), requestedIndex.length(), 0);
  dir = opendir(responsFilePath.c_str());
  dirReasHeaderSent = true;
}

void ResponseTemplate::sendDirBody(int connectionFd) {
  struct dirent *entry;
  std::string holder;
  std::stringstream ss;
  std::string len;
  if (dir) {
    while ((entry = readdir(dir)) != NULL) {
      holder += "<a href=\"";
      holder.append(entry->d_name);
      holder.append("\">").append(entry->d_name).append("</a>\n");
    }
    std::cout << holder << std::endl;
    ss << holder.length();
    ss >> len;
    len += "\r\n";
    send(connectionFd, len.c_str(), len.length(), 0);
    send(connectionFd, holder.c_str(), holder.length(), 0);
  }
  if (entry == NULL) {
    holder.clear();
    holder = "</pre>\n<hr>\n</body>\n</html>";
    ss.clear();
    ss << holder.length();
    ss >> len;
    len += "\r\n";
    send(connectionFd, len.c_str(), len.length(), 0);
    send(connectionFd, holder.c_str(), holder.length(), 0);
    send(connectionFd, "\r\n\r\n", 4, 0);
  }
}
