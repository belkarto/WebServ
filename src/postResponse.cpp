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

void Response::processResourceRequest(CLIENTIT &clientIt)
{
    std::string       uri;
    std::stringstream ss;
    std::string       fileName;
    int               ret_val;

    uri = clientIt->fields[URI];
    filePath = root + uri;
    ret_val = this->parseResourcePath(clientIt);
    if (ret_val == ERROR)
        return;
    else if (ret_val == IS_FILE)
        this->handleResourceFile(clientIt);
    else
        this->handleResourceDire(clientIt);
}
