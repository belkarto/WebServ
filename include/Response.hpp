#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Response
{
  public:
      std::string status;               
      std::string connection; 
      std::string contentType;
      std::string contentLength;
      std::string filePath;

      void  setErrorResponse(CLIENTIT& clientIt);
      void  setGetResponse(CLIENTIT& clientIt);
      void  setPostResponse(CLIENTIT& clientIt);
      void  setDeleteResponse(CLIENTIT& clientIt);
};
#endif
