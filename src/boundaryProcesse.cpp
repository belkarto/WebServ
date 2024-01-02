#include "../include/Multiplexer.hpp"
#include <unistd.h>

void Response::getUnprocessedHeaders(CLIENTIT &clientIt)
{
    char *startPos = NULL, *tmp = NULL;
    int   leftData;

    if (firstBuffer)
    {
        startPos = std::strstr(clientIt->header_buffer, "\r\n\r\n");
        if (startPos == NULL || (startPos + 4) - clientIt->header_buffer == request_read)
        {
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
            tmp = new char[leftData + 1];
            std::memcpy(tmp, startPos, leftData);
            std::memcpy(clientIt->header_buffer, tmp, leftData);
            delete[] tmp;
        }
        tmp = std::strstr(clientIt->header_buffer, "\r\n\r\n");
        if (tmp == NULL)
        {
            delete[] clientIt->header_buffer;
            clientIt->header_buffer = NULL;
        }
        firstBuffer = false;
    }
    else
    {
        char *tmp = NULL;

        tmp = std::strstr(clientIt->header_buffer, "Content-Type:");
        if (tmp == NULL)
        {
            delete[] clientIt->header_buffer;
            clientIt->header_buffer = NULL;
        }
        else
        {
            std::stringstream ss(tmp);
            std::string       line;
            ss >> line >> line;
            std::map<std::string, std::string>::iterator it = clientIt->fields.find("Content-Type");
            if (it != clientIt->fields.end())
                clientIt->fields.erase(it);
            clientIt->setContentType(line);
            unprocessedHeadersDone = true;
        }
    }
}
