#include "Multiplexer.hpp"

void	Multiplexer::parseRequestHeaders(std::vector<Client>::iterator& clientIt)
{
	size_t  							pos, offset, sep;
	std::string							header, key, value;
	std::stringstream					ss;
	std::vector<std::string>::iterator	it;

	offset = 0;
    while ((pos = clientIt->headers.find("\r\n", offset)) != std::string::npos)
	{
		header = clientIt->headers.substr(offset, pos - offset);
		if ((sep = header.find(':')) != std::string::npos)
		{
			ss << header;
			getline(ss, key, ':');
			ss >> value;
			if ((it = std::find(headers_fields.begin(), headers_fields.end(), key)) != headers_fields.end())
				fields_setters[ptrdiff_t (it - headers_fields.begin())];
		}
		else
		{
			errorHandler(clientIt, 400, "Bad request");
			return;
		}
	}
}

void	Multiplexer::parseRequestLine(std::vector<Client>::iterator& clientIt, size_t pos)
{
	std::stringstream	ss;
	std::string			method, request_uri, protocol_version;

	ss << clientIt->headers;
	ss >> method >> request_uri >> protocol_version;
	clientIt->fields["method"] = method;
	clientIt->fields["request_uri"] = request_uri;
	clientIt->fields["protocol_version"] = protocol_version;
	if (method.empty() || request_uri.empty() || protocol_version.empty())
	{
		// bad request;
	}
	clientIt->headers = clientIt->headers.substr(pos + sizeof("\r\n"));
	clientIt->request_line_received = true;

}

/*
	Each header field consists of a case-insensitive field name followed by a colon (":"),
	optional leading whitespace, the field value, and optional trailing whitespace.
*/