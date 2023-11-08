#include "Multiplexer.hpp"

void	Multiplexer::parseRequestHeaders(std::vector<Client>::iterator& clientIt)
{

}

void	Multiplexer::sendResponseHeaders(std::vector<Client>::iterator& clientIt)
{
	size_t  			pos, offset, sep;
	std::string			header, key, value;
	std::stringstream	ss;

	offset = 0;
    while ((pos = clientIt->headers.find("\r\n", offset)) != std::string::npos)
	{
		header = clientIt->headers.substr(offset, pos - offset);
		if ((sep = header.find(':')) != std::string::npos)
		{
			ss << header;
			getline(ss, key, ':');
			ss >> value;
		}
		else
		{
			// 400 (bad request)			
		}
	}
}


/*
	Each header field consists of a case-insensitive field name followed by a colon (":"),
	optional leading whitespace, the field value, and optional trailing whitespace.
*/