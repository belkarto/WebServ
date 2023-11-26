#include "../include/Multiplexer.hpp"

Response::Response(void)
{
	status 			= "";   
	connection 		= "";
	contentType 	= "";
	contentLength 	= "";
	filePath 		= "";
	location 		= "";
	index 			= NULL;
    autoindex 		= false;
    fileContent 	= NULL;
	file_size 		= 0;
	readbytes 		= 0;
}

void	Response::resetState()
{
	status 			= "";   
	connection 		= "";
	contentType 	= "";
	contentLength 	= "";
	filePath 		= "";
	location 		= "";
	index 			= NULL;
    autoindex 		= false;
    fileContent 	= NULL;
	file_size 		= 0;
	readbytes 		= 0;
}

void    Response::setGetResponse(CLIENTIT& clientIt)
{
	std::string	uri;

	std::cout << __FUNCTION__ << std::endl;
	uri = clientIt->fields["request_target"];
	clientIt->serverIt->findLocation(clientIt, uri);
	if (clientIt->locatIt != clientIt->serverIt->location.end())
	{
		index = &(clientIt->locatIt->index);		// common directives
		autoindex = clientIt->locatIt->autoindex;	// to server and location
		root = clientIt->locatIt->root;				//
	}
	else
	{
		index = &(clientIt->serverIt->index);
		autoindex = clientIt->serverIt->autoindex;
		root = clientIt->serverIt->root;
	}
	std::cout << "uri: " << uri <<  std::endl;
	filePath = root + uri;
	std::cout << "filePath: " << filePath << std::endl;
	parseFilePath(clientIt);
}

void	Response::parseFilePath(CLIENTIT& clientIt)
{
	if (access(filePath.c_str(), F_OK))
	{
		this->resetState();
		status = STATUS_404;
		this->setErrorResponse(clientIt);
	}
	else if (access(filePath.c_str(), R_OK))
	{
		this->resetState();
		status = STATUS_403;
		this->setErrorResponse(clientIt);	
	}
	else 
	{
		if (is_directory(filePath.c_str()))
			handleDirectory(clientIt);
		else
			handleFile(clientIt);
	}
}

void	Response::handleDirectory(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	if (!handleIndexPages(clientIt))
	{
		if (!handleAutoIndex(clientIt))
		{
			this->resetState();
			status = STATUS_403;
			this->setErrorResponse(clientIt);
		}
	}
}

void	Response::handleFile(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	status = STATUS_200;
	generateResponse(clientIt);
}

bool	Response::handleIndexPages(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	
	std::vector<std::string>::iterator	it;

	if (!index || index->empty())
		return false;
	it = index->begin();
	for (; it != index->end(); it++)
	{
		filePath += *it;
		if (!access(filePath.c_str(), F_OK))
		{
			if (!access(filePath.c_str(), R_OK))
				handleFile(clientIt);
			else
			{
				status = STATUS_403;
				this->setErrorResponse(clientIt);
			}
			return true;
		}
	}
	return false;
}

void Response::handleURLRedirection(CLIENTIT &clientIt)
{
    (void)clientIt;
}

bool Response::handleAutoIndex(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	if (!autoindex)
		return false;
	//TODO: serve autoindex
	return true;
	(void) clientIt;
}


void	Response::generateResponse(CLIENTIT& clientIt)
{
	std::stringstream	ss;

	clientIt->start_responding = true;
	fileContent = new std::ifstream(filePath.c_str());
	file_size = getFileSize(fileContent);
	ss << file_size;
	ss >> contentLength;
	contentType = clientIt->getMimeType(filePath);
	sendHeaders(clientIt);
}

void	Response::sendHeaders(CLIENTIT& clientIt)
{
	std::string	headers;

	headers = "";
	headers += "HTTP/1.1 ";
	headers += status + CRLF;
	headers += "Content-Type: " + contentType + CRLF;
	headers += "Content-Length: " + contentLength + CRLF;
	headers += "Connection: " + connection + CRLF;
	headers += CRLF;
	send(clientIt->connect_socket, headers.c_str(), headers.length(), 0);
}

void	Response::sendBody(CLIENTIT& clientIt)
{
	char	 		buffer[CLIENT_RESPONSE_BUFFER_SIZE];
	std::streamsize	rd;

	if (readbytes < file_size)
	{
		fileContent->read(buffer, CLIENT_RESPONSE_BUFFER_SIZE);
		rd = fileContent->gcount();
		readbytes += rd;
		send(clientIt->connect_socket, &buffer, rd, 0);
	}
	else
		clientIt->response_all_sent = true;
}

void    Response::setErrorResponse(CLIENTIT& clientIt)
{
	(void)clientIt;
	std::cout << __FUNCTION__ << std::endl;
}

void Response::setPostResponse(CLIENTIT &clientIt)
{
    (void)clientIt;
}

void Response::setDeleteResponse(CLIENTIT &clientIt)
{
    (void)clientIt;
}

std::string Response::getErrorPage(int errorCode) {
  std::string finalResponse;
  std::map<int, std::string>::iterator it;

  finalResponse = "<html>" CRLF "<head><title>";
  it = Multiplexer::defErrorPages.find(errorCode);
  finalResponse.append(it->second)
      .append("</title></head>" CRLF "<body>" CRLF "<center><h1>")
      .append(it->second)
      .append("</h1></center>" CRLF);

  return finalResponse;
}
