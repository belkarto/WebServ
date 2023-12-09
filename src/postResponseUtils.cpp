#include "../include/Multiplexer.hpp"
#include <algorithm>

void Response::parseUploadPath(CLIENTIT &clientIt)
{

    if (access(filePath.c_str(), F_OK)) // file not found
    {
        if (status == STATUS_404)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_404;
        this->setErrorResponse(clientIt);
    }
    else if (access(filePath.c_str(), W_OK) || *(--filePath.end()) == '/') // permission denied
    {
        if (status == STATUS_403)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_403;
        this->setErrorResponse(clientIt);
    }
    else if (!is_directory(filePath.c_str()))
    {
        if (status == STATUS_405)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_405;
        this->setErrorResponse(clientIt);
    }
}

int Response::parseResourcePath(CLIENTIT &clientIt)
{
    if (access(filePath.c_str(), F_OK)) // file not found
    {
        // if (status == STATUS_404)
        //     return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_404;
        this->setErrorResponse(clientIt);
        return ERROR;
    }
    else if (is_directory(filePath.c_str()))
    {
        return IS_DIRECTORY;
    }
    else if (access(filePath.c_str(), W_OK) || *(--filePath.end()) == '/') // permission denied
    {
        // if (status == STATUS_403)
        //     return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_403;
        this->setErrorResponse(clientIt);
        return ERROR;
    }
    return IS_FILE;
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

void Response::ProcessUploadLocation(CLIENTIT &clientIt)
{
    std::string       uri;
    std::stringstream ss;
    std::string       fileName;
    STRINGVECTIT      it;
    // location supportes upload
    uri = clientIt->fields[URI];
    filePath = root + uri + clientIt->locatIt->upload_store;
    parseUploadPath(clientIt);
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

void Response::handleResourceFile(CLIENTIT &clientIt)
{
    if (access((filePath).c_str(), R_OK | W_OK) == 0 && !clientIt->locatIt->cgi.empty())
    {
        // if location support cgi
    }
    else
    {
        std::cout << __func__ << " " << filePath << std::endl;
        this->resetState();
        status = STATUS_403;
        this->setErrorResponse(clientIt);
        return;
    }
}

static STRINGVECTIT getIndex(STRINGVECT &indexes, std::string root)
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

void Response::handleResourceDire(CLIENTIT &clientIt)
{
    STRINGVECTIT      indexIt;

    std::cout << "request directory" << std::endl;
    // its directory
    if (*(--filePath.end()) != '/')
        filePath.append("/");
    if (clientIt->locatIt->index.empty())
    {
        this->resetState();
        status = STATUS_403;
        this->setErrorResponse(clientIt);
        return;
    }
    else
    {
        indexIt = getIndex(clientIt->locatIt->index, filePath);
        if (indexIt == clientIt->locatIt->index.end())
        {
            this->resetState();
            status = STATUS_404;
            this->setErrorResponse(clientIt);
            return;
        }
        filePath.append(*indexIt);
        this->handleResourceFile(clientIt);
    }
}
