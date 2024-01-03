#include "../include/Multiplexer.hpp"
#include <ios>
#include <unistd.h>

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
            std::string boundary;
            std::size_t found = std::string::npos;

            if (!clientIt->fields["boundary"].empty())
            {
                boundary = "--" + clientIt->fields["boundary"] + "--";
                std::string tmp(clientIt->header_buffer, clientIt->response.request_read);
                found = tmp.find(boundary);
                if (found != std::string::npos)
                {
                    tmp = tmp.substr(0, found);
                    clientIt->response.request_read = tmp.size();
                }
            }
            if (clientIt->header_buffer != NULL)
            {
                clientIt->response.outFile->write(clientIt->header_buffer, clientIt->response.request_read);
                clientIt->response.outFile->flush();
                if (found != std::string::npos)
                    request_size = 0;
                else
                    request_size -= request_read;
                delete[] clientIt->header_buffer;
                clientIt->header_buffer = NULL;
            }
        }
    }
}

static void checkUnprocessedData(char *buffer, std::streamsize &size, std::ostream *outFile, std::streamsize readed)
{
    char           *startPos;
    std::streamsize leftDataLen;

    startPos = std::strstr(buffer, "\r\n\r\n");
    if (startPos == NULL || (std::streamsize)((startPos + 4) - buffer) == readed)
        return;
    startPos += 4;
    leftDataLen = readed - (startPos - buffer);
    outFile->write(startPos, leftDataLen);
    size -= leftDataLen;
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
            unlink(outFilePath.c_str());
            unlink(filePath.c_str());
            throw std::runtime_error(STATUS_500);
        }
        ss << clientIt->fields["Content-Length"];
        ss >> request_size;
        if (request_size >= clientIt->serverIt->client_max_body_size)
        {
            clientIt->response.outFile->close();
            delete clientIt->response.outFile;
            unlink(filePath.c_str());
            unlink(outFilePath.c_str());
            throw std::runtime_error(STATUS_413);
        }
        checkUnprocessedData(clientIt->header_buffer, request_size, clientIt->response.outFile,
                             clientIt->response.request_read);
        delete[] clientIt->header_buffer;
        clientIt->header_buffer = NULL;
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
}
