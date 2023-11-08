#include "Client.hpp"


Client::Client()
{
	headers = "";
	headers_all_recieved = false;
	request_line_received = false;
	request_all_processed = false;
	response_all_sent = false;
	
	last_activity = 0;
	keepalive_requests = 0;
}

void    Client::resetState()
{
	headers = "";
	headers_all_recieved = false;
	request_line_received = false;
	request_all_processed = false;
	response_all_sent = false;
	last_activity = time(NULL);
	keepalive_requests += 1;
}
