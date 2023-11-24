// #include "Client.hpp"
#include "../include/Multiplexer.hpp"

void    Response::setErrorResponse(CLIENTIT& clientIt)
{
    std::cout << __FUNCTION__ << std::endl;
    int													code;
    std::stringstream									ss;
    std::map<std::vector<int>, std::string>::iterator	pageIt;		

    ss << status;
    ss >> code;
    if ((pageIt = clientIt->serverIt->findErrorPage(code)) != clientIt->serverIt->error_page.end())
    {
        clientIt->serverIt->findLocation(clientIt, pageIt->second);
        if (clientIt->locatIt != clientIt->serverIt->location.end())
        {
            index = &(clientIt->locatIt->index);		// common directives
            autoindex = clientIt->locatIt->autoindex;	// to server and location
            filePath = clientIt->locatIt->root + pageIt->second;
            if (!clientIt->locatIt->redirect.empty())
                handleURLRedirection(clientIt);
            else 
                handleErrorPages(clientIt);
        }
        else
        {
            index = &(clientIt->serverIt->index);
            autoindex = clientIt->serverIt->autoindex;
            filePath = clientIt->serverIt->root + pageIt->second;
            handleErrorPages(clientIt);
        }
    }
    else
    handleDefaultErrorPages(clientIt);
}

void	Response::handleErrorPages(CLIENTIT& clientIt)
{
    if (access(filePath.c_str(), F_OK))
    {
        if (status == "404 Not Found")
            return (handleDefaultErrorPages(clientIt));
        status = "404 Not Found";
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

void	Response::handleDefaultErrorPages(CLIENTIT& clientIt)
{
    std::stringstream	ss;
    std::string			code;

    ss << status;
    ss >> code;
    filePath = ERROR_PAGE_DEFAULT_LOCAT + code + ERROR_PAGE_SUFFIX;
    if (!access(filePath.c_str(), F_OK) && !access(filePath.c_str(), R_OK))
    {
        // TODO: serve Content from file
    }
    else
    {
        // TODO: serve content from defined error
    }
    (void) clientIt;
}

void	Response::handleURLRedirection(CLIENTIT& clientIt)
{
    location = clientIt->locatIt->redirect;
}

void	Response::handleDirectory(CLIENTIT& clientIt)
{
    if (!handleIndexPages(clientIt))
    {
        if (!handleAutoIndex(clientIt))
        {
            if (status == "403 Forbidden")
                return (handleDefaultErrorPages(clientIt));
            status = "403 Forbidden";
            this->setErrorResponse(clientIt);
        }
    }
}

void	Response::handleFile(CLIENTIT& clientIt)
{
    if (!access(filePath.c_str(), F_OK)) // file exists
    {
        if (!access(filePath.c_str(), R_OK)) // read permission granted
        {
            // TODO: serve content
        }
        else // permission not granted
        {
            if (status == "403 Forbidden")
                return (handleDefaultErrorPages(clientIt));
            status = "403 Forbidden";
            this->setErrorResponse(clientIt);
        }
    }
    else // file doesnt exist
    {
        if (status == "404 Not Found")
            return (handleDefaultErrorPages(clientIt));
        status = "404 Not Found";
        this->setErrorResponse(clientIt);
    }
}

bool	Response::handleIndexPages(CLIENTIT& clientIt)
{
    std::vector<std::string>::iterator	it;

    (void) clientIt;
    if (index->empty())
        return false;
    it = index->begin();
    for (; it != index->end(); it++)
    {
        if (!access(filePath.c_str(), F_OK)) // file exists
        {
            if (!access(filePath.c_str(), R_OK)) // read permission granted
            {
                // TODO: serve content
            }
            else // permission not granted
            {
                if (status == "403 Forbidden")
                {
                    handleDefaultErrorPages(clientIt);
                    return true;
                }
                status = "403 Forbidden";
                this->setErrorResponse(clientIt);
            }
            return true;
        }
        // if file doesnt exit continue searching for another index page
    }
    return false;
}

bool	Response::handleAutoIndex(CLIENTIT& clientIt)
{
    (void) clientIt;
    if (!autoindex)
        return false;
    //TODO: serve autoindex
    return true;
}


void    Response::setGetResponse(CLIENTIT& clientIt)
{
    (void) clientIt;
}

void    Response::setPostResponse(CLIENTIT& clientIt)
{
    (void) clientIt;
}

void    Response::setDeleteResponse(CLIENTIT& clientIt)
{
    (void) clientIt;
}

