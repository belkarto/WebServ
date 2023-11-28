#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"

class Response
{
  public:
	
	std::string	    	status;           // reponse headers fields  
	std::string	    	connection; 
	std::string	    	contentType;
	std::string	    	contentLength;
	std::string			transferEncoding;
	std::string	    	location;

	std::string	    	filePath;
	std::string			root;
	STRINGVECT	    	*index;
    bool		    	autoindex;

	int					code;
	std::string			special_response; 
    std::ifstream   	*fileContent;
	DIR 				*directory;
	std::streamsize		response_size;
	std::streamsize		readbytes;


	Response(void);
	~Response();
	void	resetState();

    /*		error handling				*/
    void	setErrorResponse(CLIENTIT& clientIt);
	/*		Get method					*/
    void	setGetResponse(CLIENTIT& clientIt);
	/*		POST method					*/
    void	setPostResponse(CLIENTIT& clientIt);
    /*		DELETE method					*/
	void	setDeleteResponse(CLIENTIT& clientIt);

    /*  common methods  */
    void		handleDefaultErrorPage(CLIENTIT &clientIt);
    void		parseFilePath(CLIENTIT& clientIt);
    void		handleExternalRedirection(CLIENTIT& clientIt);
    void		handleDirectory(CLIENTIT& clientIt);
    void		handleFile(CLIENTIT& clientIt);
    bool		handleIndexPages(CLIENTIT& clientIt);
    bool		handleAutoIndex(CLIENTIT& clientIt);
	void		sendHeaders(CLIENTIT& clientIt);
	void		sendResponseBuffer(CLIENTIT& clientIt);
	void		sendAutoIndexBuffer(CLIENTIT& clientIt);
	void		handleDelete(CLIENTIT& clientIt);
	void		deleteFile(CLIENTIT& clientIt);
    std::string getErrorPage(int errorCode);
};
#endif
