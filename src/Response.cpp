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
	cgiExecutable	= "";
	index 			= NULL;
    autoindex 		= false;

	code 			= 0;
	special_response = "";
	fd 				= -1;
    fileContent 	= NULL;
	directory		= NULL;
	response_size	= 0;
	readbytes 		= 0;
}

void	Response::resetState()
{
	status 			= "";   
	connection 		= "keep-alive";
	contentType 	= "text/html";
	contentLength 	= "";
	transferEncoding = "";
	location 		= "";

	filePath 		= "";
	root 			= "";
	cgiExecutable	= "";
	index 			= NULL;
    autoindex 		= false;

	code 			= 0;
	special_response = "";
	fd 				= -1;
    fileContent 	= NULL;
	directory		= NULL;
	response_size	= 0;
	readbytes 		= 0;
}

Response::~Response()
{

}

void    Response::setGetResponse(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	std::string	uri;

	uri = clientIt->fields["request_target"];
	clientIt->serverIt->findLocation(clientIt, uri);
	if (clientIt->locatIt != clientIt->serverIt->location.end())
	{
		if (find(clientIt->locatIt->method.begin(), clientIt->locatIt->method.end(), "GET")
			 == clientIt->locatIt->method.end())
		{
			status = STATUS_405;
			return (setErrorResponse(clientIt));
		}
		index = &(clientIt->locatIt->index);		// common directives
		autoindex = clientIt->locatIt->autoindex;	// to server and location
		root = clientIt->locatIt->root;				//
		if (!clientIt->locatIt->redirect.empty())
			return (handleExternalRedirection(clientIt));
		cgiExecutable = clientIt->serverIt->findCgi(clientIt, uri);
	}
	else
	{
		index = &(clientIt->serverIt->index);
		autoindex = clientIt->serverIt->autoindex;
		root = clientIt->serverIt->root;
	}
	filePath = root + uri;
	parseFilePath(clientIt);
}


void    Response::setErrorResponse(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	std::stringstream									ss;
	std::map<std::vector<int>, std::string>::iterator	pageIt;

	ss << status;
	ss >> code;
	pageIt = clientIt->serverIt->findErrorPage(code);
	if (pageIt != clientIt->serverIt->error_page.end()) // error page defined in config
	{
		if (pageIt->second[0] == '/') // internal redirection
		{
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
		handleDefaultErrorPage(clientIt);
}


void Response::setPostResponse(CLIENTIT &clientIt)
{
    (void)clientIt;
}

void Response::setDeleteResponse(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	std::string	uri;

    uri = clientIt->fields["request_target"];
	clientIt->serverIt->findLocation(clientIt, uri);
	if (clientIt->locatIt != clientIt->serverIt->location.end())
	{
		if (find(clientIt->locatIt->method.begin(), clientIt->locatIt->method.end(), "DELETE")
			 == clientIt->locatIt->method.end())
		{
			status = STATUS_405;
			return (setErrorResponse(clientIt));
		}
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
	handleDelete(clientIt);
}
