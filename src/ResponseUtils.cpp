#include "Multiplexer.hpp"

void	Response::parseFilePath(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	if (access(filePath.c_str(), F_OK)) // file not found
	{
		if (status == STATUS_404)
			return (handleDefaultErrorPage(clientIt));
		this->resetState();
		status = STATUS_404;
		this->setErrorResponse(clientIt);
	}
	else if (access(filePath.c_str(), R_OK)) // permission denied
	{
		if (status == STATUS_403)
			return (handleDefaultErrorPage(clientIt));
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
			if (status == STATUS_403) // failure in both index pages and autoindex
				return (handleDefaultErrorPage(clientIt));
			this->resetState();
			status = STATUS_403;
			this->setErrorResponse(clientIt);
		}
	}
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
				if (status == STATUS_403)
				{
					handleDefaultErrorPage(clientIt);
					return true;
				}
				status = STATUS_403;
				this->setErrorResponse(clientIt);
			}
			return true;
		}
	}
	return false;
}

void	Response::handleFile(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	status = STATUS_200;
	fileContent = new std::ifstream(filePath.c_str());
	if (!fileContent)
	{
		if (status == STATUS_500)
			handleDefaultErrorPage(clientIt);
		status = STATUS_500;
		setErrorResponse(clientIt);
	}
	response_size = getFileSize(fileContent);
	contentLength = toString(response_size);
	contentType = clientIt->getMimeType(filePath);
	sendHeaders(clientIt);
}

void Response::handleExternalRedirection(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	status = STATUS_302;
    if (location.empty()) // if location snt empty => location = errorPageURI
   	    location = clientIt->locatIt->redirect;
    special_response = "<html>" CRLF "<head><title>";
    special_response.append(STATUS_302)
					.append("</title></head>" CRLF "<body>" CRLF "<center><h1>")
					.append(STATUS_302)
					.append("</h1></center>" CRLF "</body></html>" CRLF);
	contentLength = toString(special_response.length());
	sendHeaders(clientIt);
	send(clientIt->connect_socket, &special_response[0], special_response.length(), 0);
	clientIt->response_all_sent = true;
}

bool Response::handleAutoIndex(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	if (!autoindex)
		return false;
	status = STATUS_200;
    directory = opendir(filePath.c_str());
	if (!directory)
	{
		if (status == STATUS_500)
			handleDefaultErrorPage(clientIt);
		status = STATUS_500;
		setErrorResponse(clientIt);
	}
    transferEncoding = "chunked";
	sendHeaders(clientIt);
	return true;
}

void	Response::handleDefaultErrorPage(CLIENTIT &clientIt)
{
	connection = "close";
    special_response = getErrorPage(code);
	contentLength = toString(special_response.length());
	sendHeaders(clientIt);
	send(clientIt->connect_socket, &special_response[0], special_response.length(), 0);
	clientIt->response_all_sent = true;
}

void	Response::parsePostFilePath(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	if (access(filePath.c_str(), F_OK)) // file not found
	{
		if (status == STATUS_404)
			return (handleDefaultErrorPage(clientIt));
		this->resetState();
		status = STATUS_404;
		this->setErrorResponse(clientIt);
	}
	else if (access(filePath.c_str(), W_OK) || *(--filePath.end()) == '/') // permission denied
	{
		if (status == STATUS_403)
			return (handleDefaultErrorPage(clientIt));
		this->resetState();
		status = STATUS_403;
		this->setErrorResponse(clientIt);	
	}
	else if (!is_directory(filePath.c_str()))
    {
		if (status == STATUS_405)
			return (handleDefaultErrorPage(clientIt));
		this->resetState();
		status = STATUS_405;
		this->setErrorResponse(clientIt);	
    }
}
