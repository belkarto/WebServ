#include "../include/Multiplexer.hpp"

void	Multiplexer::parseRequestHeaders(CLIENTIT& clientIt)
{
	size_t  							pos, offset, sep;
	std::string							header, key, value;
	std::stringstream					ss;
	std::vector<std::string>::iterator	it;
	std::string::iterator				last;

	offset = 0;
    std::cout << clientIt->headers << std::endl;
    while ((pos = clientIt->headers.find("\r\n", offset)) != std::string::npos)
	{
		if (pos == offset)
			return (reviewHeaders(clientIt));
		header = clientIt->headers.substr(offset, pos - offset);
		if ((sep = header.find(':')) != std::string::npos)
		{
			last = header.begin() + sep;
			if (std::find_if(header.begin(), last, isspace) != last)
				throw RequestParsingException(STATUS_400);
			ss << header;
			getline(ss, key, ':');
			getline(ss, value);
			std::transform(key.begin(), key.end(), key.begin(), tolower);
			trim(value);
			if ((it = std::find(headers_fields.begin(), headers_fields.end(), key)) != headers_fields.end())
				((*clientIt).*(Multiplexer::fields_setters)[std::ptrdiff_t(it - headers_fields.begin())])(value);
		}
		else
			throw RequestParsingException(STATUS_400);
		ss.clear();
		offset = pos + 2;
	}
	if (!offset && clientIt->headers.length() > CLIENT_HEADER_BUFFER_SIZE)
		throw RequestParsingException("431 Request Header Fields Too Large");
	clientIt->headers = clientIt->headers.substr(offset);
}

void	Multiplexer::parseRequestLine(CLIENTIT& clientIt)
{
	std::string			delim;
	std::string			method, request_target, protocol_version, blank;
	size_t				pos;
	std::stringstream	ss;

	delim = "\r\n";
	pos = clientIt->headers.find(delim);
	if (clientIt->headers.length() == CLIENT_HEADER_BUFFER_SIZE && pos == std::string::npos)
		throw RequestParsingException(STATUS_400);
	ss << clientIt->headers.substr(0, pos);	// request line
	ss >> method >> request_target >> protocol_version >> blank;	// method uri protocol_version\r\n
	if (method.empty() || request_target.empty() || protocol_version.empty() || !blank.empty())
		throw RequestParsingException(STATUS_400);
    std::cout << "request_line ==> " << ss.str() << std::endl;
	clientIt->setMethod(method);
	clientIt->setUri(request_target);
	clientIt->setProtocolVersion(protocol_version);
	clientIt->headers = clientIt->headers.substr(pos + delim.length());
	clientIt->request_line_received = true;
}

void	Multiplexer::reviewHeaders(CLIENTIT& clientIt)
{
	if (clientIt->fields.find("Host") == clientIt->fields.end())
		throw RequestParsingException(STATUS_400);
	if (clientIt->fields.find("Content-Length") != clientIt->fields.end() 
		&& clientIt->fields.find("Transfer-Encoding") != clientIt->fields.end())
		throw RequestParsingException(STATUS_400);
	if (clientIt->fields["method"] == "POST" && clientIt->fields.find("Content-Length") == clientIt->fields.end() 
		&& clientIt->fields.find("Transfer-Encoding") == clientIt->fields.end())
		throw RequestParsingException(STATUS_400);
	if (clientIt->fields.find("Connection") == clientIt->fields.end())
		clientIt->fields["Connection"] = "keep-alive";
	if (!KEEPALIVE_CONN)
	    clientIt->fields["Connection"] = "close";
	clientIt->headers_all_recieved = true;
	setServerByHost(clientIt);
	if (clientIt->fields["method"] != "POST")	// POST has to read the client's request body
		clientIt->request_all_processed = true;
}


void	Multiplexer::setServerByHost(CLIENTIT& clientIt)
{
	SERVIT	serverIt;

	serverIt = servers.begin();
	for (; serverIt != servers.end(); serverIt++)
	{
		if (clientIt->listen_socket == serverIt->listen_socket
			&& find(serverIt->server_name.begin(), serverIt->server_name.end(), clientIt->fields["Host"]) 
			!= serverIt->server_name.end())
			clientIt->serverIt = serverIt;
	}
}
