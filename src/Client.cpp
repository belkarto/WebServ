#include "Client.hpp"

std::string Client::getHeaders()
{
	size_t		readval;
	char		buffer[CHUNK];
	std::string	headers = "";

    while ((readval = read(sock.fd, buffer, CHUNK)) > 0)
    {
		headers += std::string(buffer, readval);
		if (headers.find_last_of("\r\n\r\n") != std::string::npos)
			break;
    }
	return headers;
}