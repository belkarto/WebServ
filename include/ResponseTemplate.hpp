#ifndef RESPONSE_TEMPLATE_HPP
#define RESPONSE_TEMPLATE_HPP

#include <string>

class ResponseTemplate {
public:
  std::string ResponsStatus;   // Ex: 200 OK // if errno of open ==> ENOENT not
                               // found | EACCES permission denied
  std::string server;          // webServ *server name*
  std::string ContentEncoding; // always will be set to chunked
  std::string connenction;     // keep-alive OR closed
  std::string contentType;
  std::string contentLenght;
  std::string responsFilePath;
};

#endif
