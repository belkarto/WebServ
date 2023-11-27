#include "../include/Multiplexer.hpp"

void Request::getPostRequest(CLIENTIT &client)
{
	std::string	uri;

	uri = client->fields[URI];
	client->serverIt->findLocation(client, uri);
	if (client->locatIt != client->serverIt->location.end())
	{
		indexIt = client->locatIt->index.begin();		// common directives
		root = client->locatIt->root;				//
		// if (!client->locatIt->redirect.empty())
		// 	return (handleExternalRedirection(client));
	}
    std::map<std::string, std::string>::iterator it = client->fields.begin();

    std::cout << "start sending request \n-----------------------------------------------\n" << std::endl << std::endl;
    for (; it != client->fields.end(); it++)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
}
