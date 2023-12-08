#include "../include/Multiplexer.hpp"
#include <unistd.h>

void Response::setPostResponse(CLIENTIT &clientIt)
{
    if (!this->filePathParsed)
        this->postParseFilePath(clientIt);
    else
    {
        // uploading post body
        char buffer[BUFFER_SIZE];
        int  rc = 0;

        rc = recv(clientIt->connect_socket, buffer, BUFFER_SIZE, 0);
        clientIt->response.outFile->write(buffer, rc);
        clientIt->response.outFile->flush();
        response_size -= rc;
        if (response_size <= 0)
        {
            clientIt->response.outFile->close();
            this->resetState();
            status = STATUS_201;
            this->setErrorResponse(clientIt);
            return;
            // send response
        }
    }
}

void Response::postParseFilePath(CLIENTIT &clientIt)
{
    std::string       uri;
    std::stringstream ss;
    std::string       fileName;
    STRINGVECTIT      it;

    uri = clientIt->fields[URI];
    clientIt->serverIt->findLocation(clientIt, uri);
    if (clientIt->locatIt != clientIt->serverIt->location.end())
    {
        index = &clientIt->locatIt->index;
        root = clientIt->locatIt->root;
        if (!clientIt->locatIt->redirect.empty())
            return (handleExternalRedirection(clientIt));

        // check if location suddport post request
        if (clientIt->locatIt->method.empty() ||
            clientIt->locatIt->method.end() ==
                std::find(clientIt->locatIt->method.begin(), clientIt->locatIt->method.end(), "POST"))
        {
            this->resetState();
            status = STATUS_405;
            this->setErrorResponse(clientIt); // set error to 405
            return;
        }

        // check if location support upload
        if (!clientIt->locatIt->upload_store.empty())
            this->ProcessUploadLocation(clientIt);
        else
            this->processResourceRequest(clientIt);
    }
    else
    {
        std::cerr << STATUS_405 << std::endl;
        this->resetState();
        status = STATUS_405;
        this->setErrorResponse(clientIt); // set error to 405
        return;
        // index = &(clientIt->serverIt->index);
        // root = clientIt->serverIt->root;
    }
}

STRINGVECTIT getIndex(STRINGVECT &indexes, std::string root)
{
    STRINGVECTIT it;

    it = indexes.begin();
    for (; it != indexes.end(); it++)
    {
        std::string indexPath;
        indexPath = root + *it;
        std::cout << "index " << *it << std::endl;
        std::cout << indexPath << std::endl;
        if (access(indexPath.c_str(), F_OK) == 0)
            return it;
    }
    return it;
}

void Response::processResourceRequest(CLIENTIT &clientIt)
{
    std::string       uri;
    std::stringstream ss;
    std::string       fileName;
    STRINGVECTIT      indexIt;
    int               ret_val;

    uri = clientIt->fields[URI];
    filePath = root + uri;
    ret_val = this->parseResourcePath(clientIt);
    if (ret_val == ERROR)
        return;
    else if (ret_val == IS_FILE)
    {
        std::cout << "reques file" << std::endl;
        if (!clientIt->locatIt->cgi.empty())
        {
            // if location support cgi
        }
        else
        {
            this->resetState();
            status = STATUS_403;
            this->setErrorResponse(clientIt);
            return;
        }
    }
    else
    {
        std::cout << "request directory" << std::endl;
        // its directory
        if (*(--filePath.end()) != '/')
        {
            // add / to the end of request and make redirection to new uri 301 response
            // clientIt->locatIt->redirect = clientIt->serverIt->bind_addr_str + uri + "/";
            // std::cout << clientIt->locatIt->redirect << std::endl;
            // return (handleExternalRedirection(clientIt));
        }
        else
        {
            std::cout << filePath << std::endl;
            if (clientIt->locatIt->index.empty())
            {
                std::cout << "error no index provided" << std::endl;
                this->resetState();
                status = STATUS_403;
                this->setErrorResponse(clientIt);
                return;
            }
            else
            {
                std::cout << filePath << std::endl;
                // search indexes

                indexIt = getIndex(clientIt->locatIt->index, filePath);
                if (indexIt == clientIt->locatIt->index.end())
                {
                    std::cout << "index file not found" << std::endl;
                    this->resetState();
                    status = STATUS_404;
                    this->setErrorResponse(clientIt);
                    return;
                }
                if (access((filePath + *indexIt).c_str(), R_OK | W_OK) != 0 || clientIt->locatIt->cgi.empty())
                {
                    this->resetState();
                    status = STATUS_403;
                    this->setErrorResponse(clientIt);
                    return;
                }
                else
                {
                    std::cout << "not cgi implemented yet" << std::endl;
                    exit(1);
                    // run cgi on requested file
                }
            }
        }
    }
}
