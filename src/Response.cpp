#include "../include/Multiplexer.hpp"

Response::Response(void)
{
	status 			= "";   
	connection 		= "keep-alive";
	contentType 	= "text/html";
	contentLength 	= "";
	transferEncoding = "";
	location 		= "";

	filePath 		= "";
	root 			= "";
	index 			= NULL;
    autoindex 		= false;

	code 			= 0;
	special_response = "";
    fileContent 	= NULL;
	directory		= NULL;
	response_size	= 0;
	readbytes 		= 0;
}

void	Response::resetState()
{
	status 			= "";   
	connection 		= "";
	contentType 	= "";
	contentLength 	= "";
	transferEncoding = "";
	location 		= "";

	filePath 		= "";
	root 			= "";
	index 			= NULL;
    autoindex 		= false;

	code 			= 0;
	special_response = "";
    fileContent 	= NULL;
	directory		= NULL;
	response_size	= 0;
	readbytes 		= 0;
}

void    Response::setGetResponse(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	std::string	uri;

	uri = clientIt->fields["request_target"];
	clientIt->serverIt->findLocation(clientIt, uri);
	if (clientIt->locatIt != clientIt->serverIt->location.end())
	{
		index = &(clientIt->locatIt->index);		// common directives
		autoindex = clientIt->locatIt->autoindex;	// to server and location
		root = clientIt->locatIt->root;				//
		if (!clientIt->locatIt->redirect.empty())
			return (handleExternalRedirection(clientIt));
	}
	else
	{
		index = &(clientIt->serverIt->index);
		autoindex = clientIt->serverIt->autoindex;
		root = clientIt->serverIt->root;
	}
	filePath = root + uri;
	std::cout << "uri: " << uri <<  std::endl;
	std::cout << "filePath: " << filePath << std::endl;
	parseFilePath(clientIt);
}


void    Response::setErrorResponse(CLIENTIT& clientIt)
{

	/*
		This causes an internal redirect to the specified uri with the client request method changed to “GET” 
		(for all methods other than “GET” and “HEAD”).
	*/
	std::cout << __FUNCTION__ << std::endl;
	std::cout << status << std::endl;
	std::stringstream									ss;
	std::map<std::vector<int>, std::string>::iterator	pageIt;

	ss << status;
	ss >> code;
	pageIt = clientIt->serverIt->findErrorPage(code);
	if (pageIt != clientIt->serverIt->error_page.end())
	{
		// error page defined;
		if (pageIt->second[0] == '/')
		{
			// internal redirection
			clientIt->fields["request_target"] = pageIt->second;
			setGetResponse(clientIt);
		}
		else // external redirection
		{
			location = pageIt->second;
			handleExternalRedirection(clientIt);
		}
	}
	else
		handleDefaultPage(clientIt);
}


void Response::setPostResponse(CLIENTIT &clientIt)
{
    (void)clientIt;
}

void Response::setDeleteResponse(CLIENTIT &clientIt)
{
    (void)clientIt;
}

