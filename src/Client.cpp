#include "Client.hpp"


Client::Client()
{
	headers = "";
	headers_all_recieved = false;
	request_line_received = false;
	request_all_processed = false;
	response_all_sent = false;
	error_occurred = false;
}

void    Client::resetState()
{

}


void	Client::setHost(std::string &host)
{
	fields["host"] = host;
}

void	Client::setContentType(std::string &content_type)
{
	std::map<std::string, std::string>::iterator	it;

	if ((it = Multiplexer::mime_types.find(content_type)) != Multiplexer::mime_types.end())
		fields["Content-Type"] = content_type;
	// else
	// unsupported media error

}

void	Client::setContentLength(std::string &content_length)
{
	std::string::iterator it;

	if ((std::find_if(content_length.begin(), content_length.end(), not_digit)) == content_length.end())
		fields["Content-Length"] = content_length;
	//else
	//bad request
}

void	Client::setConnection(std::string &connection)
{
	if (connection == "close" || connection == "keep-alive")
		fields["Connection"] = connection;
	// else
		// bad request
}
