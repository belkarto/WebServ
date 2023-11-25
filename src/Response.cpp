// #include "Client.hpp"
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

void    Response::setErrorResponse(CLIENTIT& clientIt)
{
    int code;
    std::stringstream ss;
    std::map<std::vector<int>, std::string>::iterator pageIt;

	// in case of error we close connection
	std::cout << __FUNCTION__ << std::endl;
	connection = "close";
	ss << status;
	ss >> code;
	if ((pageIt = clientIt->serverIt->findErrorPage(code)) != clientIt->serverIt->error_page.end())
	{
		clientIt->serverIt->findLocation(clientIt, pageIt->second);
		if (clientIt->locatIt != clientIt->serverIt->location.end())
		{
			index = &(clientIt->locatIt->index);		// common directives
			autoindex = clientIt->locatIt->autoindex;	// to server and location
			root = clientIt->locatIt->root;				//
			filePath = clientIt->locatIt->root + pageIt->second;
	        if (!clientIt->locatIt->redirect.empty())
		        handleURLRedirection(clientIt);
            else 
		        handlePage(clientIt);
		}
		else
		{
			index = &(clientIt->serverIt->index);
			autoindex = clientIt->serverIt->autoindex;
			root = clientIt->serverIt->root;
			filePath = clientIt->serverIt->root + pageIt->second;
			handlePage(clientIt);
		}
	}
	else
		handleDefaultErrorPages(clientIt);
}

void	Response::handlePage(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	if (access(filePath.c_str(), F_OK))
	{
		if (status == STATUS_404)
			return (handleDefaultErrorPages(clientIt));
		status = STATUS_404;
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

void Response::handleDefaultErrorPages(CLIENTIT &clientIt)
{
    std::stringstream ss;
    std::string code;

    ss << status;
    ss >> code;
    // TODO: serve default errror pages
    (void)clientIt;
}

void Response::handleURLRedirection(CLIENTIT &clientIt)
{
	location = clientIt->locatIt->redirect;
    (void)clientIt;
}

void Response::handleDirectory(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	if (!handleIndexPages(clientIt))
	{
		if (!handleAutoIndex(clientIt))
		{
			if (status == STATUS_403)
				return (handleDefaultErrorPages(clientIt));
			status = STATUS_403;
			this->setErrorResponse(clientIt);
		}
	}
}

void Response::handleFile(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	if (!access(filePath.c_str(), F_OK)) // file exists
	{
		if (!access(filePath.c_str(), R_OK)) // read permission granted
			generateResponse(clientIt);
		else // permission not granted
		{
			if (status == STATUS_403)
				return (handleDefaultErrorPages(clientIt));
			status = STATUS_403;
			this->setErrorResponse(clientIt);
		}
	}
	else // file doesnt exist
	{
		if (status == STATUS_404)
			return (handleDefaultErrorPages(clientIt));
		status = STATUS_404;
		this->setErrorResponse(clientIt);
	}
}

bool Response::handleIndexPages(CLIENTIT &clientIt)
{
    std::vector<std::string>::iterator it;

	std::cout << __FUNCTION__  << std::endl;
	if (index->empty())
		return false;
	for (it = index->begin(); it != index->end(); it++)
	{
		filePath = root + *it;
		if (!access(filePath.c_str(), F_OK)) // file exists
		{
			if (!access(filePath.c_str(), R_OK)) // read permission granted
				generateResponse(clientIt);
			else // permission not granted
			{
				if (status == STATUS_403)
				{
					handleDefaultErrorPages(clientIt);
					return true;
				}
				status = STATUS_403;
				this->setErrorResponse(clientIt);
			}
			return true;
		}
	} // if file doesnt exit continue searching for another index page
	return false;
    (void)clientIt;
}

bool Response::handleAutoIndex(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	if (!autoindex)
		return false;
	//TODO: serve autoindex
	(void) clientIt;
	return true;
}


void	Response::generateResponse(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	std::stringstream	ss;

	clientIt->start_responding = true;
	std::cout << "PATH: " << filePath << std::endl;
	std::cout << status << std::endl;
	fileContent = new std::ifstream(filePath.c_str());
	file_size = getFileSize(fileContent);
	ss << file_size;
	ss >> contentLength;
	contentType = clientIt->getMimeType(filePath);
	sendHeaders(clientIt);
}

void	Response::sendHeaders(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	std::string	headers;

	headers = "";
	headers += "HTTP/1.1 ";
	headers += status + CRLF;
	headers += "Content-Type: " + contentType + CRLF;
	headers += "Content-Length: " + contentLength + CRLF;
	headers += "Connection: " + connection + CRLF;
	headers += CRLF;

	std::cout << headers << std::endl;
	send(clientIt->connect_socket, headers.c_str(), headers.length(), 0);
}

void	Response::sendBody(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;
	const size_t	chunk = 1024;
	char	 		buffer[chunk];
	std::streamsize	rd;

	if (readbytes < file_size)
	{
		fileContent->read(buffer, chunk);
		rd = fileContent->gcount();
		readbytes += rd;
		send(clientIt->connect_socket, &buffer, rd, 0);
	}
	else
		clientIt->response_all_sent = true;
}



void    Response::setGetResponse(CLIENTIT& clientIt)
{
    (void)clientIt;
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
