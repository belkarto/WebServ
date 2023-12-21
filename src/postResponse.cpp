#include "../include/Multiplexer.hpp"

void Response::setPostResponse(CLIENTIT &clientIt)
{
    if (!this->filePathParsed)
    {
        try
        {
            this->postParseFilePath(clientIt);
        }
        catch (std::exception &e)
        {
            std::cout << "got exception " << e.what() << std::endl;
            this->resetState();
            status = e.what();
            this->setErrorResponse(clientIt);
            return;
        }
    }
    else
    {
        // uploading post body
        if (response_size <= 0)
        {
            if (postCgi)
            {
                Multiplexer::env = setPostCgiEnv(Multiplexer::env, clientIt);
                cgiExecutable = clientIt->serverIt->findCgi(clientIt, filePath);
                if (!cgiExecutable.empty())
                {
                    cgi = true;
                    handleCgi(clientIt, POST);
                }
                else
                {
                    resetState();
                    status = STATUS_403;
                    this->setErrorResponse(clientIt);
                    return;
                }
                // std::cout << "request require CGI" << std::endl;
                // exit(100);
            }
            else
            {
                clientIt->response.outFile->close();
                this->resetState();
                status = STATUS_201;
                this->setErrorResponse(clientIt);
                return;
            }
        }
        else
        {
            char buffer[BUFFER_SIZE];
            int  rc = 0;

            rc = recv(clientIt->connect_socket, buffer, BUFFER_SIZE, 0);
            clientIt->response.outFile->write(buffer, rc);
            clientIt->response.outFile->flush();
            response_size -= rc;
        }
    }
}

static void checkUnprocessedData(char *buffer, std::streamsize &size, std::ostream *outFile)
{
    char *startPos;
    int   leftDataLen;

    startPos = std::strstr(buffer, "\r\n\r\n");
    if (startPos == NULL || (startPos + 4) - buffer == CLIENT_HEADER_BUFFER_SIZE)
        return;
    startPos += 4;
    leftDataLen = CLIENT_HEADER_BUFFER_SIZE - (startPos - buffer);
    outFile->write(startPos, leftDataLen);
    size -= leftDataLen;
    delete[] buffer;
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
            throw std::runtime_error(STATUS_500);
        ss << clientIt->fields["Content-Length"];
        ss >> response_size;
        if (response_size >= clientIt->serverIt->client_max_body_size)
            throw std::runtime_error(STATUS_413);
        checkUnprocessedData(clientIt->header_buffer, response_size, clientIt->response.outFile);
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
    outFilePath = "/tmp" + clientIt->generateFileName(clientIt->fields["Content-Type"]);
    clientIt->response.outFile = new std::ofstream(outFilePath.c_str());
    std::cout << "file name is " << outFilePath << std::endl;
}
