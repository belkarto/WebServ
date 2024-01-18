#include "../include/Multiplexer.hpp"

void Response::setPostResponse(CLIENTIT &clientIt)
{
    try
    {
        if (!this->filePathParsed)
            this->postParseFilePath(clientIt);
        else
            handleRequestBody(clientIt);
    }
    catch (std::exception &e)
    {
        delete[] clientIt->header_buffer;
        this->resetState();
        status = e.what();
        this->setErrorResponse(clientIt);
        return;
    }
}

void Response::handleRequestBody(CLIENTIT &clientIt)
{
    if (clientIt->fields["Transfer-Encoding"] == "chunked")
        recvChunkedBody(clientIt);
    else
        recvRequestBody(clientIt);
}

void Response::handlePostResponse(CLIENTIT &clientIt)
{
    clientIt->response.outFile->close();
    delete clientIt->response.outFile;
    clientIt->response.outFile = NULL;
    if (postCgi)
    {
        cgi = true;
        handleCgi(clientIt);
    }
    else
        throw std::runtime_error(STATUS_201);
}

void Response::recvRequestBody(CLIENTIT &clientIt)
{
    if (clientIt->header_buffer != NULL)
    {
        if (request_read > request_body_size)
            clientIt->response.outFile->write(clientIt->header_buffer, request_body_size);
        else
            clientIt->response.outFile->write(clientIt->header_buffer, clientIt->response.request_read);
        clientIt->response.outFile->flush();
        request_body_size -= request_read;
        delete[] clientIt->header_buffer;
        clientIt->header_buffer = NULL;
        if (request_body_size <= 0)
            handlePostResponse(clientIt);
    }
}

static void checkUnprocessedData(CLIENTIT &clientIt)
{
    char *startPos;

    startPos = std::strstr(clientIt->header_buffer, "\r\n\r\n");
    if (startPos == NULL)
    {
        return;
    }
    startPos += 4;
    clientIt->response.request_read -= (startPos - clientIt->header_buffer);

    std::memmove(clientIt->header_buffer, startPos, clientIt->response.request_read);
    clientIt->header_buffer[clientIt->response.request_read] = '\0';
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

        // check if location support post request
        if (clientIt->locatIt->method.empty() ||
            clientIt->locatIt->method.end() ==
                std::find(clientIt->locatIt->method.begin(), clientIt->locatIt->method.end(), "POST"))
            throw std::runtime_error(STATUS_405);

        // check if location support upload
        if (!clientIt->locatIt->upload_store.empty())
            this->ProcessUploadLocation(clientIt);
        else
            this->processResourceRequest(clientIt);

        if (!clientIt->response.outFile->is_open())
        {
            clientIt->response.outFile->close();
            delete clientIt->response.outFile;
            clientIt->response.outFile = NULL;
            unlink(outFileCgiPath.c_str());
            unlink(filePath.c_str());
            throw std::runtime_error(STATUS_500);
        }
        checkUnprocessedData(clientIt);
        this->filePathParsed = true;
    }
    else
        throw std::runtime_error(STATUS_405);
}

void Response::processResourceRequest(CLIENTIT &clientIt)
{
    std::string       uri;
    std::stringstream ss;
    std::string       fileName;
    int               ret_val;

    uri = clientIt->fields[URI];
    filePath = root + uri;
    ret_val = this->parseResourcePath();
    if (ret_val == IS_FILE)
        this->handleResourceFile(clientIt);
    else
        this->handleResourceDire(clientIt);
    outFileCgiPath = "/tmp" + clientIt->generateFileName(clientIt->fields["Content-Type"]);
    clientIt->response.outFile = new std::ofstream(outFileCgiPath.c_str());
}

char **Response::setCgiEnv(CLIENTIT &clientIt)
{
    cgiEnv[0] = strdup(("SERVER_PROTOCOL=" + clientIt->fields["protocol_version"]).c_str());
    cgiEnv[1] = strdup(("CONTENT_LENGTH=" + clientIt->fields["Content-Length"]).c_str());
    cgiEnv[2] = strdup(("CONTENT_TYPE=" + clientIt->fields["Content-Type"]).c_str());
    cgiEnv[3] = strdup(("HTTP_HOST=" + clientIt->serverIt->bind_addr_str).c_str());
    cgiEnv[4] = strdup(("HTTP_COOKIE=" + clientIt->fields["Cookie"]).c_str());
    cgiEnv[5] = strdup(("REQUEST_METHOD=" + clientIt->fields["method"]).c_str());
    cgiEnv[6] = strdup(("REDIRECT_STATUS=" + status).c_str());
    cgiEnv[7] = strdup(("SCRIPT_FILENAME=" + clientIt->response.filePath).c_str());
    cgiEnv[8] = strdup(("PATH_INFO=" + clientIt->response.filePath).c_str());
    cgiEnv[9] = NULL;
    return cgiEnv;
}
