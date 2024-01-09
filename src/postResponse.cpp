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
            delete[] clientIt->header_buffer;
            this->resetState();
            status = e.what();
            this->setErrorResponse(clientIt);
            return;
        }
    }
    else
    {
        if (request_size == 0)
        {
            clientIt->response.outFile->close();
            delete clientIt->response.outFile;
            clientIt->response.outFile = NULL;
            if (postCgi)
            {
                cgiExecutable = clientIt->serverIt->findCgi(clientIt, filePath);
                if (!cgiExecutable.empty())
                {
                    cgi = true;
                    handleCgi(clientIt);
                }
                else
                {
                    resetState();
                    status = STATUS_403;
                    this->setErrorResponse(clientIt);
                    return;
                }
            }
            else
            {
                this->resetState();
                status = STATUS_201;
                this->setErrorResponse(clientIt);
                return;
            }
        }
        else
        {
            if (!clientIt->header_buffer)
                return;
            if (!clientIt->fields["boundary"].empty())
            {
                // function to handle boundary
                this->prossesBoundaryBuffer(clientIt);
            }
            else
            {
                clientIt->response.outFile->write(clientIt->header_buffer, clientIt->response.request_read);
                clientIt->response.outFile->flush();
                request_size -= request_read;
                delete[] clientIt->header_buffer;
                clientIt->header_buffer = NULL;
            }
        }
    }
}

static void checkUnprocessedData(CLIENTIT &cliIt)
{
    char           *startPos;
    std::streamsize leftDataLen;
    std::string     tmp_str;

    if (!cliIt->header_buffer)
        return;
    startPos = std::strstr(cliIt->header_buffer, "\r\n\r\n");
    if (startPos == NULL || (std::streamsize)((startPos + 4) - cliIt->header_buffer) == cliIt->response.request_read)
    {
        std::cout << RED << "no boundary found" << RESET << std::endl;
        cliIt->response.request_size -= cliIt->response.request_read;
        cliIt->response.filePathParsed = true;
        delete[] cliIt->header_buffer;
        cliIt->header_buffer = NULL;
        return;
    }
    startPos += 4;
    leftDataLen = cliIt->response.request_read - (startPos - cliIt->header_buffer);
    if (!cliIt->fields["boundary"].empty())
    {
        tmp_str.assign(startPos, leftDataLen);
        std::string boundary = "--" + cliIt->fields["boundary"];
        std::size_t found = tmp_str.find(boundary);
        if (found != std::string::npos)
        {
            if (found != tmp_str.find(boundary + "--"))
            {
                /* 
                 * if boundary is found but not the last one
                 */
                cliIt->response.outFile->write(startPos, found - 2);
                cliIt->response.outFile->flush();
                cliIt->response.outFile->close();
                char *holder = new char[cliIt->response.request_read];
                cliIt->response.request_size -= (found + cliIt->fields["boundary"].size() + 4);
                cliIt->response.request_read -= found;
                std::memcpy(holder, startPos + found, cliIt->response.request_read);
                std::memset(cliIt->header_buffer, 0, CLIENT_HEADER_BUFFER_SIZE);
                std::memcpy(cliIt->header_buffer, holder, cliIt->response.request_read);
                delete[] holder;
                delete cliIt->response.outFile;
                return;
            }
            else
            {
                cliIt->response.outFile->write(startPos, found - 2);
                cliIt->response.outFile->flush();
                cliIt->response.request_size -= (found + cliIt->fields["boundary"].size() + 4);
                std::cout << GREEN <<  cliIt->response.request_size << RESET << std::endl;
                cliIt->response.request_size = 0;
                cliIt->response.filePathParsed = true;
                delete[] cliIt->header_buffer;
                cliIt->header_buffer = NULL;
                return;
            }
        }
    }
    cliIt->response.outFile->write(startPos, leftDataLen);
    cliIt->response.outFile->flush();
    cliIt->response.request_size -= leftDataLen;
    cliIt->response.filePathParsed = true;
    delete[] cliIt->header_buffer;
    cliIt->header_buffer = NULL;
}

void Response::postParseFilePath(CLIENTIT &clientIt)
{
    std::string       uri;
    std::stringstream ss;
    std::string       fileName;
    STRINGVECTIT      it;

    uri = clientIt->fields[URI];
    if (!clientIt->fields["boundary"].empty())
        getUnprocessedHeaders(clientIt);
    if (!unprocessedHeadersDone && !clientIt->fields["boundary"].empty())
        return;
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
            unlink(outFilePath.c_str());
            unlink(filePath.c_str());
            throw std::runtime_error(STATUS_500);
        }
        if (request_size >= clientIt->serverIt->client_max_body_size)
        {
            clientIt->response.outFile->close();
            delete clientIt->response.outFile;
            clientIt->response.outFile = NULL;
            unlink(filePath.c_str());
            unlink(outFilePath.c_str());
            throw std::runtime_error(STATUS_413);
        }
        checkUnprocessedData(clientIt);
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
    cgiEnv[8] = strdup(("PATH_INFO=" + filePath).c_str());
    cgiEnv[9] = NULL;
    return cgiEnv;
}
