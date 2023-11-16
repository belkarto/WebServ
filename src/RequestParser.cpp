#include "Multiplexer.hpp"

void	Multiplexer::parseRequestHeaders(CLIENTIT& clientIt)
{
	size_t  							pos, offset, sep;
	std::string							header, key, value;
	std::stringstream					ss;
	std::vector<std::string>::iterator	it;
	std::string::iterator				last;

	offset = 0;
    while ((pos = clientIt->headers.find("\r\n", offset)) != std::string::npos)
	{
		if (pos == offset)
		{
			clientIt->headers_all_recieved = true;
			return ;
		}
		header = clientIt->headers.substr(offset, pos - offset);
		if ((sep = header.find(':')) != std::string::npos)
		{
			last = header.begin() + sep;
			if (std::find_if(header.begin(), last, isspace) != last)
				throw RequestParsingException("400 Bad Request");
			ss << header;
			getline(ss, key, ':');
			std::transform(key.begin(), key.end(), key.begin(), tolower);
			getline(ss, value);
			trim(value);
			if ((it = std::find(headers_fields.begin(), headers_fields.end(), key)) != headers_fields.end())
				((*clientIt).*(Multiplexer::fields_setters)[std::ptrdiff_t(it - headers_fields.begin())])(value);
		}
		else
			throw RequestParsingException("400 Bad Request");
		offset = pos + 2;
	}
	if (!offset && clientIt->headers.length() > CLIENT_HEADER_BUFFER_SIZE)
		throw RequestParsingException("431 Request Header Fields Too Large");
	clientIt->headers = clientIt->headers.substr(offset);
}

void	Multiplexer::parseRequestLine(CLIENTIT& clientIt)
{
	std::string			delim;
	std::string			method, request_uri, protocol_version, blank;
	size_t				pos;
	std::stringstream	ss;

	delim = "\r\n";
	if (clientIt->headers.length() == CLIENT_HEADER_BUFFER_SIZE 
		&& (pos = clientIt->headers.find(delim)) == std::string::npos)
		throw RequestParsingException("414 URI Too Long");
	ss << clientIt->headers.substr(0, pos);	// request line
	ss >> method >> request_uri >> protocol_version >> blank;	// method uri protocol_version\r\n
	if (method.empty() || request_uri.empty() || protocol_version.empty() || !blank.empty())
		throw RequestParsingException("400 Bad Request");
	clientIt->fields["request_uri"] = request_uri;
	clientIt->setMethod(method);
	clientIt->setProtocolVersion(protocol_version);
	clientIt->headers = clientIt->headers.substr(pos + delim.length());
	clientIt->request_line_received = true;
}
