#include "../include/Multiplexer.hpp"

void Response::setPostResponse(CLIENTIT &clientIt)
{
    if (!this->filePathParsed)
        this->postParseFilePath(clientIt);
    else
    {
        char buffer[BUFFER_SIZE];
        int  rc = 0;

        while ((rc = recv(clientIt->connect_socket, buffer, BUFFER_SIZE, 0)))
        {
            clientIt->response.outFile->write(buffer, rc);
            clientIt->response.outFile->flush();
            response_size -= rc;
            if (response_size <= 0)
            {
                clientIt->response.outFile->close();
                // send response 
            }
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
    std::cout << leftDataLen << std::endl;
    outFile->write(startPos, leftDataLen);
    size -= leftDataLen;
    delete[] buffer;
}

void Response::postParseFilePath(CLIENTIT &clientIt)
{
    std::string       uri;
    std::stringstream ss;

    uri = clientIt->fields[URI];
    clientIt->serverIt->findLocation(clientIt, uri);
    if (clientIt->locatIt != clientIt->serverIt->location.end())
    {
        index = &clientIt->locatIt->index;
        root = clientIt->locatIt->root;
        if (!clientIt->locatIt->redirect.empty())
            return (handleExternalRedirection(clientIt));
    }
    else
    {
        index = &(clientIt->serverIt->index);
        root = clientIt->serverIt->root;
    }
    filePath = root + uri;

    parsePostFilePath(clientIt);
    filePath.append("/" + clientIt->generateFileName(clientIt->fields["Content-Type"]));
    clientIt->response.outFile = new std::ofstream(filePath.c_str());
    if (!clientIt->response.outFile)
    {
        this->resetState();
        status = STATUS_500;
        this->setErrorResponse(clientIt);
        return;
    }
    ss << clientIt->fields["Content-Length"];
    ss >> response_size;
    if (response_size >= clientIt->serverIt->client_max_body_size)
    {
        this->resetState();
        status = STATUS_413;
        this->setErrorResponse(clientIt);
        return;
    }
    checkUnprocessedData(clientIt->header_buffer, response_size, clientIt->response.outFile);
    this->filePathParsed = true;
}
