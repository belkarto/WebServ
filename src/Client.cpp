#include "Client.hpp"

Client::Client()
{
	headers = "";
	headers_all_recieved = false;
	request_line_received = false;
	request_all_processed = false;
	response_all_sent = false;
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
	else
		throw RequestParsingException("400 Bad Request");
}

void	Client::setContentLength(std::string &content_length)
{
	std::string::iterator it;

	if ((std::find_if(content_length.begin(), content_length.end(), not_digit)) == content_length.end())
		fields["Content-Length"] = content_length;
	else
		throw RequestParsingException("400 Bad Request");

}

void	Client::setConnection(std::string &connection)
{
	if (connection == "close" || connection == "keep-alive")
		fields["Connection"] = connection;
	else
		throw RequestParsingException("400 Bad Request");
}

void	Client::setProtocolVersion(std::string &protocol_version)
{
	std::stringstream	ss;
	std::string			prefix, major, minor;
	int					val1, val2;
	size_t				pos;

	if (protocol_version.find("HTTP/") != 0)
		throw RequestParsingException("400 Bad Request");
	pos = protocol_version.find('.', strlen("HTTP/"));
	if (pos != std::string::npos && pos == protocol_version.length() - 1)
		throw RequestParsingException("400 Bad Request");
	ss << protocol_version;
	std::getline(ss, prefix, '/');
	std::getline(ss, major, '.');
	ss >> minor;
	try
	{
		val1 = ft_stoi(major.c_str());
		if (!minor.empty())
			val2 = ft_stoi(minor.c_str());
	}
	catch(std::exception &e)
	{
		throw RequestParsingException("400 Bad Request");
	}
	if (protocol_version != "HTTP/1.1")
		throw RequestParsingException("505 HTTP Version Not Supported");
	fields["protocol_version"] = protocol_version;
}

void	Client::setMethod(std::string &method)
{
	std::vector<std::string>			methods;
	std::vector<std::string>::iterator	it;
	const char	*http_methods[] = {
		"GET",
		"POST",
		"DELETE",
		"HEAD",
		"PUT",
		"CONNECT",
		"OPTIONS",
	};

	methods.assign(http_methods, http_methods + sizeof(http_methods) / sizeof(const char*));
	if ((it = std::find(methods.begin(), methods.end(), method)) != methods.end()
		&& std::ptrdiff_t (it - methods.begin()) > 2)
		throw RequestParsingException("501 Not Implemented");
	else if (it == methods.end())
		throw  RequestParsingException("400 Bad Request");
	fields["method"] = method;
}
