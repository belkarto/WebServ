#include "Client.hpp"


Client::Client()
{
	headers = "";
	header_timeout = time(NULL);
	headers_recieved = false;
	request_line_received = false;
	method = "";
	request_uri = "";
}

void    Client::resetState()
{

}

bool	Client::headerTimedout()
{
	time_t	elapsed;

	elapsed = time(NULL) - this->header_timeout;
	return (elapsed > CLIENT_HEADER_timeout);
}