#include "Multiplexer.hpp"

void	Response::parseFilePath(CLIENTIT& clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	if (access(filePath.c_str(), F_OK)) // file not found
	{
		if (status == STATUS_404)
			return (handleDefaultPage(clientIt));
		this->resetState();
		status = STATUS_404;
		this->setErrorResponse(clientIt);
	}
	else if (access(filePath.c_str(), R_OK)) // permission denied
	{
		if (status == STATUS_403)
			return (handleDefaultPage(clientIt));
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
			// failure in both index pages and autoindex
			if (status == STATUS_403)
				return (handleDefaultPage(clientIt));
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
            std::cout << "index: " << filePath << std::endl;
			if (!access(filePath.c_str(), R_OK))
				handleFile(clientIt);
			else
			{
				if (status == STATUS_403)
				{
					handleDefaultPage(clientIt);
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
	generateResponse(clientIt);
}

void Response::handleExternalRedirection(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	status = STATUS_302;
    code = 302;
    // in case the function got called from setGetResponse => location will be empty
    // else => location = errorPageURI
    if (location.empty())
   	    location = clientIt->locatIt->redirect;
    special_response = getErrorPage(code);
	generateResponse(clientIt);
}

bool Response::handleAutoIndex(CLIENTIT &clientIt)
{
	std::cout << __FUNCTION__ << std::endl;

	if (!autoindex)
		return false;
	//TODO: serve autoindex
    directory = opendir(filePath.c_str());
    transferEncoding = "chunked";
    generateResponse(clientIt);
	return true;
}

void	Response::handleDefaultPage(CLIENTIT &clientIt)
{
    special_response = getErrorPage(code);
    generateResponse(clientIt);
}
