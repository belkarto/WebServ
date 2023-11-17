#include "Client.hpp"

Client::Client()
{
	headers = "";
	headers_all_recieved = false;
	request_line_received = false;
	request_all_processed = false;
	response_all_sent = false;
  response_template_set = false;
}

void    Client::resetState()
{

}

void	Client::setTransferEncoding(std::string &encoding)
{
	fields["Transfer-Encoding"] = encoding;
}

void	Client::setContentType(std::string &content_type)
{
	fields["Content-Type"] = content_type;
}

void	Client::setHost(std::string &host)
{
	fields["host"] = host;
}

void	Client::setContentLength(std::string &content_length)
{
	fields["Content-Length"] = content_length;
}

void	Client::setConnection(std::string &connection)
{
	fields["Connection"] = connection;
}

void	Client::setProtocolVersion(std::string &protocol_version)
{
	std::stringstream		ss;
	std::string				prefix, major, minor;
	size_t					dotpos;

	if (protocol_version.find("HTTP/"))	// start with "HTTP/"
		throw RequestParsingException("400 Bad Request");
	dotpos = protocol_version.find('.', strlen("HTTP/"));
	if (dotpos != std::string::npos && dotpos == protocol_version.length() - 1)
		throw RequestParsingException("400 Bad Request");
	ss << protocol_version;
	std::getline(ss, prefix, '/');
	std::getline(ss, major, '.');
	ss >> minor;
	if (major.empty() || std::find_if(major.begin(), major.end(), not_digit) != major.end()
		|| (!minor.empty() && std::find_if(minor.begin(), minor.end(), not_digit) != minor.end()))
		throw RequestParsingException("400 Bad Request");
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
		"Trace",
		"Patch"
	};

	methods.assign(http_methods, http_methods + sizeof(http_methods) / sizeof(const char*));
	if ((it = std::find(methods.begin(), methods.end(), method)) != methods.end()
		&& std::ptrdiff_t (it - methods.begin()) > 2)
		throw RequestParsingException("501 Not Implemented");
	else if (it == methods.end())
		throw  RequestParsingException("400 Bad Request");
	fields["method"] = method;
}
