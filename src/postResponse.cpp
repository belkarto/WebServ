#include "../include/Multiplexer.hpp"
#include <algorithm>

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
            std::cout << "served" << std::endl;
            return;
            // send response
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
        if (clientIt->locatIt->method.end() ==
            std::find(clientIt->locatIt->method.begin(), clientIt->locatIt->method.end(), "POST"))
        {
            this->resetState();
            status = STATUS_405;
            this->setErrorResponse(clientIt); // set error to 405
            return;
        }
    }
    else
    {
        index = &(clientIt->serverIt->index);
        root = clientIt->serverIt->root;
    }
    filePath = root + uri + clientIt->locatIt->upload_store;
    std::cout << filePath << std::endl;
    parsePostFilePath(clientIt);
    fileName = clientIt->generateFileName(clientIt->fields["Content-Type"]);
    clientIt->response.fileLocation = uri + fileName;
    filePath.append(fileName);
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
