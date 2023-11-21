#include "Client.hpp"

void    Response::setErrorResponse(CLIENTIT& clientIt)
{
	std::map<std::vector<int>, std::string>::iterator	it,ite;
	std::stringstream									ss;
	int													code;

	ss << status;
	ss >> code;
	it = clientIt->serverIt->error_page.begin();
	ite = clientIt->serverIt->error_page.end();
	for (; it != ite && find(it->first.begin(), it->first.end(), code) == it->first.end(); it++ );
	filePath = (it != ite) ? it->second : it->second;
}

void    Response::setGetResponse(CLIENTIT& clientIt)
{
	(void) clientIt;
}

void    Response::setPostResponse(CLIENTIT& clientIt)
{
	(void) clientIt;
}

void    Response::setDeleteResponse(CLIENTIT& clientIt)
{
	(void) clientIt;
}
