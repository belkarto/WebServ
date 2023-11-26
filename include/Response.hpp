#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"

class Response
{
  public:
	std::string	    	status;             
	std::string	    	connection; 
	std::string	    	contentType;
	std::string	    	contentLength;
	std::string	    	filePath;
	std::string	    	location;
	STRINGVECT	    	*index;
    bool		    	autoindex;
	std::string			root;
    std::ifstream   	*fileContent;
	std::streamsize		file_size;
	std::streamsize		readbytes;


	Response(void);
	void	resetState();
    /*		error handling				*/
    void	setErrorResponse(CLIENTIT& clientIt);
    void	handleDefaultErrorPages(CLIENTIT& clientIt);
    std::string getErrorPage(int errorCode);
	/*		Get method					*/
    void	setGetResponse(CLIENTIT& clientIt);
	/*		POST method					*/
    void	setPostResponse(CLIENTIT& clientIt);
    /*		DELETE method					*/
	void	setDeleteResponse(CLIENTIT& clientIt);

    /*  common methods  */
    void	parseFilePath(CLIENTIT& clientIt);
    void	handleURLRedirection(CLIENTIT& clientIt);
    void	handleDirectory(CLIENTIT& clientIt);
    void	handleFile(CLIENTIT& clientIt);
    bool	handleIndexPages(CLIENTIT& clientIt);
    bool	handleAutoIndex(CLIENTIT& clientIt);
	void	generateResponse(CLIENTIT& clientIt);
	void	sendHeaders(CLIENTIT& clientIt);
	void	sendBody(CLIENTIT& clientIt);
};
#endif
