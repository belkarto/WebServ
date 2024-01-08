#include "../include/Multiplexer.hpp"

void Response::parseUploadPath()
{
    if (access(filePath.c_str(), F_OK)) // file not found
        throw std::runtime_error(STATUS_404);
    else if (access(filePath.c_str(), W_OK) || *(--filePath.end()) == '/') // permission denied
        throw std::runtime_error(STATUS_403);
    else if (!is_directory(filePath.c_str()))
        throw std::runtime_error(STATUS_405);
}

int Response::parseResourcePath()
{
    if (access(filePath.c_str(), F_OK)) // file not found
        throw std::runtime_error(STATUS_404);
    else if (is_directory(filePath.c_str()))
        return IS_DIRECTORY;
    else if (access(filePath.c_str(), W_OK) || *(--filePath.end()) == '/') // permission denied
        throw std::runtime_error(STATUS_403);
    return IS_FILE;
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
    parseUploadPath();
    fileName = clientIt->generateFileName(clientIt->fields["Content-Type"]);
    filePath.append(fileName);
    clientIt->response.fileLocation = uri + fileName;
    clientIt->response.outFile = new std::ofstream(filePath.c_str());
}

void Response::handleResourceFile(CLIENTIT &clientIt)
{
    if (access((filePath).c_str(), R_OK | W_OK) == 0 && !clientIt->locatIt->cgi.empty())
        this->postCgi = true;
    else
        throw std::runtime_error(STATUS_403);
}

static STRINGVECTIT getIndex(STRINGVECT &indexes, std::string root)
{
    STRINGVECTIT it;

    it = indexes.begin();
    for (; it != indexes.end(); it++)
    {
        std::string indexPath;
        indexPath = root + *it;
        if (access(indexPath.c_str(), F_OK) == 0)
            return it;
    }
    return it;
}

void Response::handleResourceDire(CLIENTIT &clientIt)
{
    STRINGVECTIT indexIt;

    if (*(--filePath.end()) != '/')
        filePath.append("/");
    if (clientIt->locatIt->index.empty())
        throw std::runtime_error(STATUS_403);
    else
    {
        indexIt = getIndex(clientIt->locatIt->index, filePath);
        if (indexIt == clientIt->locatIt->index.end())
            throw std::runtime_error(STATUS_404);
        filePath.append(*indexIt);
        this->handleResourceFile(clientIt);
    }
}

void Response::getUnprocessedHeaders(CLIENTIT &clientIt)
{
    char *startPos = NULL, *tmp = NULL;
    int   leftData;

    if (firstBuffer)
    {
        startPos = std::strstr(clientIt->header_buffer, "\r\n\r\n");
        if (startPos == NULL || (startPos + 4) - clientIt->header_buffer == request_read)
        {
            request_size -= request_read;
            delete[] clientIt->header_buffer;
            clientIt->header_buffer = NULL;
            firstBuffer = false;
            return;
        }
        startPos += 4;
        leftData = (request_read) - (startPos - clientIt->header_buffer);
        if (leftData > 0)
        {
            request_read = leftData;
            request_size -= leftData;
            tmp = new char[leftData + 1];
            std::memcpy(tmp, startPos, leftData);
            std::memcpy(clientIt->header_buffer, tmp, leftData);
            delete[] tmp;
        }
        tmp = std::strstr(clientIt->header_buffer, "\r\n\r\n");
        if (tmp == NULL)
        {
            request_size -= std::strlen(clientIt->header_buffer);
            delete[] clientIt->header_buffer;
            clientIt->header_buffer = NULL;
        }
        firstBuffer = false;
    }
    else
    {
        char                                        *tmp = NULL;
        std::stringstream                            ss;
        std::string                                  line;
        std::map<std::string, std::string>::iterator it = clientIt->fields.find("Content-Type");

        if (!clientIt->header_buffer)
            return;
        tmp = std::strstr(clientIt->header_buffer, "Content-Type:");
        if (tmp == NULL)
        {
            std::cerr << YELLOW << "REQ " << request_size << " " << request_read << RESET << std::endl;
            std::cout.write(clientIt->header_buffer, request_read) << std::endl << std::endl;
            // exit(22);
            if (std::strstr(clientIt->header_buffer, "\r\n\r\n"))
            {
                if (it != clientIt->fields.end())
                    clientIt->fields.erase(it);
                clientIt->setContentType(line);
                unprocessedHeadersDone = true;
            }
            {
                request_size -= std::strlen(clientIt->header_buffer);
                delete[] clientIt->header_buffer;
                clientIt->header_buffer = NULL;
                unprocessedHeadersDone = false;
            }
        }
        else
        {
            char *crlf;
            ss << tmp;
            ss >> line >> line;
            if (it != clientIt->fields.end())
                clientIt->fields.erase(it);
            clientIt->setContentType(line);
            crlf = std::strstr(clientIt->header_buffer, "\r\n\r\n");
            if (crlf == NULL)
            {
                request_size -= std::strlen(clientIt->header_buffer);
                delete[] clientIt->header_buffer;
                clientIt->header_buffer = NULL;
            }
            else
                request_size -= (crlf + 4) - clientIt->header_buffer;
            unprocessedHeadersDone = true;
        }
    }
}

void Response::prossesBoundaryBuffer(CLIENTIT &clientIT)
{
    std::string boundary = "--" + clientIT->fields["boundary"];
    std::string tmp_str;
    std::size_t found;

    tmp_str.assign(clientIT->header_buffer, request_read);
    found = tmp_str.find(boundary);
    if (found != std::string::npos)
    {
        if (found != tmp_str.find(boundary + "--"))
        {
            clientIT->response.outFile->write(clientIT->header_buffer, found - 2);
            clientIT->response.outFile->flush();
            clientIT->response.request_size -= (found + clientIT->fields["boundary"].size() + 2);
            char *holder = new char[request_read];
            std::memcpy(holder, clientIT->header_buffer + found, request_read - found);
            std::memset(clientIT->header_buffer, 0, CLIENT_HEADER_BUFFER_SIZE);
            std::memcpy(clientIT->header_buffer, holder, request_read - found);
            request_read -= found;
            delete[] holder;
            clientIT->response.outFile->close();
            delete clientIT->response.outFile;
            clientIT->response.outFile = NULL;
            clientIT->response.filePathParsed = false;
            return;
        }
        else
        {
            clientIT->response.outFile->write(clientIT->header_buffer, found - 2);
            clientIT->response.outFile->flush();
            clientIT->response.request_size -= (found + clientIT->fields["boundary"].size() + 4);
            clientIT->response.request_size = 0;
            delete[] clientIT->header_buffer;
            clientIT->header_buffer = NULL;
            return;
        }
    }
    else
    {
        clientIT->response.outFile->write(clientIT->header_buffer, request_read);
        clientIT->response.outFile->flush();
        clientIT->response.request_size -= request_read;
        delete[] clientIT->header_buffer;
        clientIT->header_buffer = NULL;
    }
}
