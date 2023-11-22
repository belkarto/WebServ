#ifndef RESPONSE_TEMPLATE_HPP
#define RESPONSE_TEMPLATE_HPP

#include <string>
#include <dirent.h>

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

  void sendTopHead(int);
  void sendDirBody(int);
  void reset();
};

#endif
