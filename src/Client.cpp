#include "Client.hpp"


Client::Client()
{
	headers = "";
	header_timeout = time(NULL);
	headers_all_recieved = false;
	request_line_received = false;
	method = "";
	request_uri = "";
	last_activity = 0;
	keepalive_requests = 0;
	response_all_sent = false;
}

void    Client::resetState()
{
	headers = "";
	header_timeout = time(NULL);
	headers_all_recieved = false;
	request_line_received = false;
	method = "";
	request_uri = "";
	response_all_sent = false;
}

bool	Client::headerTimedout()
{
	time_t	elapsed;

	elapsed = time(NULL) - this->header_timeout;
	return (elapsed > CLIENT_HEADER_timeout);
}