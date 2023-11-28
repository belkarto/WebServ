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

    std::map<std::string, std::string>::iterator it = clientIt->fields.begin();

    std::cout << "\n-----------------------------------------------\n" << std::endl << std::endl;
    for (; it != clientIt->fields.end(); it++)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
    std::cout << "\n-----------------------------------------------\n" << std::endl << std::endl;
    std::cout << "file ----> " << filePath << "\n" << std::endl;
    parsePostFilePath(clientIt);
    // filePath.append("/" + clientIt->generateFileName(clientIt->fields["Content-Type"]));
    std::cout << "file ----> " << filePath << std::endl;
}
