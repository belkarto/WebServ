#include "../include/Multiplexer.hpp"

Response::Response(void)
{
	status 			= "";
	connection		= "";
	contentType 	= "text/html";
	contentLength 	= "";
	transferEncoding = "";
	location 		= "";

	filePath 		= "";
	root 			= "";
	index 			= NULL;
    autoindex 		= false;

	cgiExecutable	= "";
	CgiFilePath		= "";
	cgi				= false;

	special_response = "";
    fileContent 	= NULL;
	directory		= NULL;
	response_size	= 0;
	request_size	= 0;
	readbytes 		= 0;
    filePathParsed  = false;
    
    postCgi         = false;
    firstBuffer     = true;
    unprocessedHeadersDone = false;
}

void	Response::resetState()
{
	status 			= "";
	connection		= "";
	contentType 	= "text/html";
	contentLength 	= "";
	transferEncoding = "";
	location 		= "";

	filePath 		= "";
	root 			= "";
	index 			= NULL;
    autoindex 		= false;

	cgiExecutable	= "";
	CgiFilePath		= "";
	cgi				= false;

	special_response = "";
    fileContent 	= NULL;
	directory		= NULL;
	response_size	= 0;
	request_size	= 0;
	readbytes 		= 0;
    filePathParsed  = false;
    
    postCgi         = false;
    firstBuffer     = true;
    unprocessedHeadersDone = false;
}

Response::~Response()
{

}

void    Response::setGetResponse(CLIENTIT& clientIt)
{
	std::string	uri;

	uri = clientIt->fields["request_target"];
	clientIt->serverIt->findLocation(clientIt, uri);
	if (clientIt->locatIt != clientIt->serverIt->location.end())
	{
		if (find(clientIt->locatIt->method.begin(), clientIt->locatIt->method.end(), "GET")
			 == clientIt->locatIt->method.end())
		{
			if (status == STATUS_405)
				return (handleDefaultErrorPage(clientIt));
			this->resetState();
			status = STATUS_405;
			return (setErrorResponse(clientIt));
		}
		index = &(clientIt->locatIt->index);		// common directives
		autoindex = clientIt->locatIt->autoindex;	// to server and location
		root = clientIt->locatIt->root;				//
		if (!clientIt->locatIt->redirect.empty())
			return (handleExternalRedirection(clientIt));
		cgiExecutable = clientIt->serverIt->findCgi(clientIt, uri);
		if (!cgiExecutable.empty())
			cgi = true;
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

	std::stringstream									ss;
	std::map<std::vector<int>, std::string>::iterator	pageIt;
	int													code;

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

void Response::setDeleteResponse(CLIENTIT &clientIt)
{

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
