#ifndef RESPONSE_TEMPLATE_HPP
#define RESPONSE_TEMPLATE_HPP

#include "Multiplexer.hpp"
#include <string>

typedef struct {
  std::string fileName;
} autoindexData;

class ResponseTemplate {
public:
  ResponseTemplate();

  bool headersSent;
  bool dirResponse;
  bool dirReasHeaderSent;
  DIR *dir;
  std::string ResponsStatus;
  std::string ContentEncoding;
  std::string connenction;
  std::string contentType;
  std::string transferEncoding;
  std::string contentLenght;
  std::string responsFilePath;
  std::map<int, std::string> errorPages;

  void sendTopHead(int);
  void sendDirBody(int);
  void reset();

  std::string getErrorPage(int errorCode);
};

#endif
