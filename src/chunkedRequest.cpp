#include "../include/Multiplexer.hpp"

    // if (request_read == 0)
    // {
    //     delete [] clientIt->header_buffer;
    //     clientIt->header_buffer = NULL;
    //     return;
    // }
void Response::recvChunkedBody(CLIENTIT &clientIt)
{
    std::stringstream ss;
    std::streamsize        toWrite;

    std::cout << "request_read: " << request_read << std::endl;
    if (chunk_size == 0)
    {
        ss << clientIt->header_buffer;
        ss >> std::hex >> chunk_size;
        if (chunk_size == 0)
        {
            std::cout.write(clientIt->header_buffer, request_read) << std::endl;
            delete [] clientIt->header_buffer;
            clientIt->header_buffer = NULL;
            handlePostResponse(clientIt);
        }
        char *crlf = strstr(clientIt->header_buffer, "\r\n");
        if (crlf == NULL)
        {
            delete [] clientIt->header_buffer;
            clientIt->header_buffer = NULL;
            return;
        }
        crlf += 2;
        request_read -= (crlf - clientIt->header_buffer);
        std::memmove(clientIt->header_buffer, crlf, request_read);
        clientIt->header_buffer[request_read] = '\0';
    }
    if (request_read > chunk_size)
        toWrite = chunk_size;
    else
        toWrite = request_read;
    clientIt->response.outFile->write(clientIt->header_buffer, toWrite);
    clientIt->response.outFile->flush();
    chunk_size -= toWrite;
    if (chunk_size != 0)
    {
        delete [] clientIt->header_buffer;
        clientIt->header_buffer = NULL;
    }
    else
    {
        char *tmp = new char[chunk_size + 1];
        std::memmove(tmp, clientIt->header_buffer + toWrite, chunk_size);
        delete [] clientIt->header_buffer;
        clientIt->header_buffer = tmp;
    }
}
