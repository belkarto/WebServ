#include "../include/Multiplexer.hpp"

void Response::setPostResponse(CLIENTIT &clientIt)
{
    std::cout << __FUNCTION__ << std::endl;

    std::string uri;

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

    std::cout << "uri: " << uri << std::endl;
    std::cout << "filePath: " << filePath << std::endl;

    parsePostFilePath(clientIt);
    filePath.append("/" + clientIt->generateFileName(clientIt->fields["Content-Type"]));
    clientIt->response.outFile = new std::ofstream(filePath.c_str());
    if (!clientIt->response.outFile)
    {
		if (status == STATUS_500)
			return (handleDefaultErrorPage(clientIt));
		this->resetState();
		status = STATUS_500;
		this->setErrorResponse(clientIt);	
    }

    std::stringstream ss;

    ss << clientIt->fields["Content-Length"];
    ss >> response_size;
    
    char buffer[BUFFER_SIZE];
    int rc = 0;

    while ((rc = recv(clientIt->connect_socket, buffer, BUFFER_SIZE, 0)))
    {
        clientIt->response.outFile->write(buffer, rc);
        response_size -= rc;
        if (response_size <= 0)
            exit(2);
    }
}
