#include "../include/Multiplexer.hpp"

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
        if (clientIt->locatIt->method.empty() ||
            clientIt->locatIt->method.end() ==
                std::find(clientIt->locatIt->method.begin(), clientIt->locatIt->method.end(), "POST"))
        {
            this->resetState();
            status = STATUS_405;
            this->setErrorResponse(clientIt); // set error to 405
            return;
        }
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
    (void)clientIt;
    // get requested resource F_OK W_OK
    // if request is file ==> if location has CGI else 405
    // if request is directory ==>           it has / in the end           ==> if directory has index ==> if
    // location has CGI else 405
    //                         ==> else make redirection with / in the end ==> else 405
}
