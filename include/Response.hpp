#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Response
{
  public:
		std::string	status;             
		std::string	connection; 
		std::string	contentType;
		std::string	contentLength;
		std::string	filePath;
		std::string	location;
		STRINGVECT	*index;
    bool		    autoindex;

      void	setErrorResponse(CLIENTIT& clientIt);
      void	setGetResponse(CLIENTIT& clientIt);
      void	setPostResponse(CLIENTIT& clientIt);
      void	setDeleteResponse(CLIENTIT& clientIt);
      void	handleCustomErrorPages(CLIENTIT& clientIt);
      void	handleDefaultErrorPages(CLIENTIT& clientIt);
      void	handleURLRedirection(CLIENTIT& clientIt);
      void	handleDirectory(CLIENTIT& clientIt);
      void	handleFile(CLIENTIT& clientIt);
      bool	handleIndexPages(CLIENTIT& clientIt);
      bool	handleAutoIndex(CLIENTIT& clientIt);
};
#endif
