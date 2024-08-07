#include "ConfigParser.hpp"

void ConfigParser::parseServerBlock()
{
	std::string line;
	std::vector<std::string>::iterator it;

	while (std::getline(config, line))
	{
		std::stringstream ss(line);
		split(directive_components, ss);
		if (directive_components.empty())
			continue;
		if (!block_delem.first)
		{
			if (directive_components[0] != "{" || directive_components.size() > 1)
				throw ConfigFileParsingException("invalid server block syntax");
			block_delem.first = true;
		}
		else
		{
			if ((it = std::find(server_directives.begin(), server_directives.end(), directive_components[0])) == server_directives.end())
			{
				if (directive_components[0] == "}" && directive_components.size() == 1)
					block_delem.second = true;
				else
					throw ConfigFileParsingException("invalid directive name");
				return;
			}
			// all directive besides location should ends with ; 
			if (directive_components[0] != "location")
				checkDirectiveSyntax();
			(this->*server_parsers[std::ptrdiff_t(it - server_directives.begin())])();
		}
	}
}

bool ConfigParser::checkServerSyntax()
{
	block_delem.first = true;
	block_delem.second = false;
	if (directive_components.size() > 2)
		return false;
	if (directive_components[0] != "server" && directive_components[0] != "server{")
		return false;
	if (directive_components.size() == 2 && (directive_components[0] != "server" || directive_components[1] != "{"))
		return false;
	if (directive_components.size() == 1 && directive_components[0] == "server")
		block_delem.first = false;
	return true;
}

void	ConfigParser::parseListen()
{
	std::string	host, port;

	freeaddrinfo(_servers.back().bind_addr);
	_servers.back().bind_addr = NULL;
	if (directive_components.size() > 1)
		throw ConfigFileParsingException("invalid number of arguments in listen directive");
	std::stringstream	ss(directive_components[0]);
	std::getline(ss, host, ':');
	ss >> port;
	if (host.empty() || port.empty() 
		|| !addr_resolver(&_servers.back().bind_addr, host.c_str(), port.c_str()))
		throw ConfigFileParsingException("invalid host or port in listen directive");
	/* 
		getaddrinfo accept ip addr in the following format
		a.b.c.d
		a.b.c
		a.b			127.0.0.1 <=> 127.0.1 <=> 127.1 <=> localhost
		a
		the use of socket_addr_ntop to unify these formats
	*/
	_servers.back().bind_addr_str = socket_addr_ntop(_servers.back().bind_addr);
}

void	ConfigParser::parseServerName()
{
	_servers.back().server_name = directive_components;
}

void	ConfigParser::parseClientMaxBodySize()
{
	std::string::iterator	it;
	std::string				value;

	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in client_max_body_size directive");
	value =  directive_components.back();
	it = std::find_if(value.begin(), value.end(), not_digit);
	if (it != value.end() && (!isunit(*it) || it != value.end() - 1))
			throw ConfigFileParsingException("invalid value in client_max_body_size directive");
	try
	{
		if (it != value.end())	// in case a unit was provided
			_servers.back().client_max_body_size = converttobytes(ft_stoll(value.c_str()), std::tolower(*it));
		else
			_servers.back().client_max_body_size = ft_stoll(value.c_str());
	}
	catch(const std::exception& e)
	{
		throw ConfigFileParsingException("invalid value in client_max_body_size directive");
	}
}

void	ConfigParser::parseErrorPages()
{
	std::vector<int>					codes;
	int									code;
	std::vector<std::string>::iterator	it;

	// error_page directive should have at least two args => a code and a path
	if (directive_components.size() < 2)
		throw ConfigFileParsingException("invalid number of arguments in error_page directive");
	try
	{
		it = directive_components.begin();
		for (; it != directive_components.end() - 1; it++)
		{
			code = ft_stoi(it->c_str());
			if (code < 400 || code > 600)
				throw ConfigFileParsingException("invalid code value in error_page directive");
			codes.push_back(code);
		}
		_servers.back().error_page[codes] = directive_components.back();
	}
	catch(const std::exception& e)
	{
		throw ConfigFileParsingException("invalid code value in error_page directive");
	}
}

void	ConfigParser::parseIndex()
{
	_servers.back().index = directive_components;		 
}

void	ConfigParser::parseRoot()
{
	// root directive should have only one arg
	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in root directive");
	_servers.back().root = directive_components.back();
}

void	ConfigParser::parseAutoIndex()
{
	std::string	value;

	// autoindex directive should have only one arg => on or off
	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in autoindex directive");
	value = directive_components.back();
	if (value == "on")
		_servers.back().autoindex = true;
	else if (value == "off")
		_servers.back().autoindex = false;
	else
		throw ConfigFileParsingException("invalid value in autoindex directive");
}