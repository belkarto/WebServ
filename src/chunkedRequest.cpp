#include "../include/Multiplexer.hpp"

static size_t getChunkSize(char **buffer, std::streamsize &readed)
{
    char *tmp = *buffer;
    char *crlf = strstr(tmp, "\r\n");
    if (crlf == NULL)
        throw std::runtime_error(STATUS_400);

    size_t chunk_size = 0;
    if (tmp[0] == '\r' && tmp[1] == '\n')
    {
        crlf = strstr(tmp + 2, "\r\n");
        if (crlf == NULL)
            throw std::runtime_error(STATUS_400);

        std::istringstream ss(std::string(tmp + 2, crlf - (tmp + 2)));
        ss >> std::hex >> chunk_size;
    }
    else
    {
        std::istringstream ss(std::string(tmp, crlf - tmp));
        ss >> std::hex >> chunk_size;
    }

    crlf += 2;
    readed -= (crlf - *buffer);
    std::memmove(*buffer, crlf, readed);
    return chunk_size;
}

static void joinBuffers(char *&buffer, char *leftOverBuffer, std::streamsize &readed, std::streamsize leftOverSize)
{
    if (buffer == NULL)
    {
        buffer = new char[leftOverSize + 1];
        std::memcpy(buffer, leftOverBuffer, leftOverSize);
        buffer[leftOverSize] = '\0';
        return;
    }
    char *tmp = buffer;
    buffer = new char[readed + leftOverSize + 1];
    std::memcpy(buffer, leftOverBuffer, leftOverSize);
    std::memcpy(buffer + leftOverSize, tmp, readed);
    readed += leftOverSize;
    buffer[readed] = '\0';
    delete[] tmp;
}

void Response::recvChunkedBody(CLIENTIT &clientIt)
{
    if (clientIt->header_buffer == NULL && !leftOver)
        return;
    // in case of after removing the request headers the buffer is empty
    if (chunk_size == 0)
    {
        if (leftOver)
        {
            joinBuffers(clientIt->header_buffer, leftOverBuffer, request_read, leftOverSize);
            delete[] leftOverBuffer;
            leftOverBuffer = NULL;
            leftOver = false;
        }
        chunk_size = getChunkSize(&clientIt->header_buffer, request_read);
        if (chunk_size == 0)
            handlePostResponse(clientIt);
    }
    if (request_read < chunk_size)
    {
        clientIt->response.outFile->write(clientIt->header_buffer, request_read);
        clientIt->response.outFile->flush();
        request_body_size += request_read;
        chunk_size -= request_read;
        delete[] clientIt->header_buffer;
        clientIt->header_buffer = NULL;
    }
    else
    {
        clientIt->response.outFile->write(clientIt->header_buffer, chunk_size);
        clientIt->response.outFile->flush();
        request_body_size += chunk_size;
        request_read -= chunk_size;
        if (request_read == 0 || request_read == 2)
        {
            delete[] clientIt->header_buffer;
            clientIt->header_buffer = NULL;
            chunk_size = 0;
            return;
        }
        char *tmp = clientIt->header_buffer + chunk_size;
        leftOverSize = request_read;
        leftOver = true;
        leftOverBuffer = new char[request_read + 1];
        std::memcpy(leftOverBuffer, tmp, request_read);
        leftOverBuffer[request_read] = '\0';
        delete[] clientIt->header_buffer;
        clientIt->header_buffer = NULL;
        chunk_size = 0;
    }
    if (request_body_size > clientIt->serverIt->client_max_body_size)
    {
        clientIt->response.outFile->close();
        delete clientIt->response.outFile;
        clientIt->response.outFile = NULL;
        unlink(filePath.c_str());
        throw std::runtime_error(STATUS_413);
    }
}
