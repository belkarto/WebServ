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
            /*
             * copy the unprocessed data to the header_buffer
             * and delete the old header_buffer
             */
            // request_size -= leftData;
            tmp = new char[leftData + 1];
            std::memcpy(tmp, startPos, leftData);
            std::memcpy(clientIt->header_buffer, tmp, leftData);
            delete[] tmp;
        }
        tmp = std::strstr(clientIt->header_buffer, "\r\n\r\n");
        if (tmp == NULL)
        {
            request_size -= leftData;
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
        /*
         * check if the header_buffer contains the Content-Type header
         * if not, then the header_buffer contains only the body
         * so we need to read more data from the socket
         */
        if (tmp == NULL)
        {
            std::cerr << YELLOW << "REQ " << request_size << " " << request_read << RESET << std::endl;
            // std::cout.write(clientIt->header_buffer, request_read) << std::endl << std::endl;
            // exit(22);
            if (std::strstr(clientIt->header_buffer, "\r\n\r\n"))
            {
                if (it != clientIt->fields.end())
                    clientIt->fields.erase(it);
                clientIt->setContentType(line);
                unprocessedHeadersDone = true;
            }
            else
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
            std::cout << request_size << " " << request_read << std::endl;
            if (it != clientIt->fields.end())
                clientIt->fields.erase(it);
            clientIt->setContentType(line);
            crlf = std::strstr(clientIt->header_buffer, "\r\n\r\n");
            if (crlf == NULL)
            {
                request_size -= request_read;
                std::cout << ORANGE << "req read " << request_read << " heade len " << std::strlen(clientIt->header_buffer) << RESET << std::endl;
                delete[] clientIt->header_buffer;
                clientIt->header_buffer = NULL;
            }
            else
            {
                std::cout << ORANGE << "B " << request_size << " " << request_read << RESET << std::endl;
                request_size -= (crlf + 4) - clientIt->header_buffer;
                std::cout << ORANGE << "A " << request_size << " " << request_read << RESET << std::endl;
            }
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
            outFile->write(clientIT->header_buffer, found - 2);
            outFile->flush();
            request_size -= (found + clientIT->fields["boundary"].size() + 2);
            request_read -= found;
            char *holder = new char[request_read];
            std::memcpy(holder, clientIT->header_buffer + found, request_read);
            std::memset(clientIT->header_buffer, 0, CLIENT_HEADER_BUFFER_SIZE);
            std::memcpy(clientIT->header_buffer, holder, request_read);
            delete[] holder;
            outFile->close();
            delete outFile;
            outFile = NULL;
            filePathParsed = false;
            return;
        }
        else
        {
            outFile->write(clientIT->header_buffer, found - 2);
            outFile->flush();
            request_size -= (found + clientIT->fields["boundary"].size() + 4);
            request_size = 0;
            delete[] clientIT->header_buffer;
            clientIT->header_buffer = NULL;
            return;
        }
    }
    else
    {
        outFile->write(clientIT->header_buffer, request_read);
        outFile->flush();
        request_size -= request_read;
        delete[] clientIT->header_buffer;
        clientIT->header_buffer = NULL;
    }
}
