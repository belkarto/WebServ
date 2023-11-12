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
				fields_setters[std::ptrdiff_t (it - headers_fields.begin())];
		}
		else
			throw RequestParsingException("400 Bad Request");
	}
}

void	Multiplexer::parseRequestLine(std::vector<Client>::iterator& clientIt)
{
	std::stringstream					ss;
	std::string							method, request_uri, protocol_version, blank;

	ss << clientIt->headers;
	ss >> method >> request_uri >> protocol_version >> blank;
	if (method.empty() || request_uri.empty() || protocol_version.empty() || !blank.empty())
		throw RequestParsingException("400 Bad Request");
	clientIt->fields["request_uri"] = request_uri;
	clientIt->setMethod(method);
	clientIt->setProtocolVersion(protocol_version);
}

/*
	Each header field consists of a case-insensitive field name followed by a colon (":"),
	optional leading whitespace, the field value, and optional trailing whitespace.
*/
