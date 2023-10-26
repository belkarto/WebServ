#include "ConfigParser.hpp"

const char	*ConfigParser::directives[NUM_DIREC] = {
	"listen",
	"server_name",
	"root",
	"index",
	"client_max_body_size",
	"error_page",
	"location",
	"autoindex",
};

void (ConfigParser::*ConfigParser::direct_parsers[NUM_DIREC])(void) = {
    &ConfigParser::parseListen,
    &ConfigParser::parseServerName,
    &ConfigParser::parseRoot,
    &ConfigParser::parseIndex,
    &ConfigParser::parseClientMaxBodySize,
    &ConfigParser::parseErrorPages,
    &ConfigParser::parseLocations,
    &ConfigParser::parseAutoIndex,
};

void     ConfigParser::checkDirectiveSyntax()
{
    if (directive_components.back() != ";" && directive_components.back().back() == ';')
    {
        directive_components.back().erase(directive_components.back().end() - 1);
        directive_components.push_back(";");
    }
    if (directive_components.back() != ";" || directive_components.size() < 3)
        throw ConfigFileParsingException("invalid server directive syntax");
	directive_components.pop_back();
	directive_components.erase(directive_components.begin());
}

bool	ConfigParser::checkServerSyntax()
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

void	ConfigParser::parseServerBlock()
{
	std::string							line;
	std::vector<std::string>::iterator	it;

	while (std::getline(config, line))
	{
		std::stringstream	ss(line);
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
			if ((it = std::find(directives_vect.begin(), directives_vect.end(), directive_components[0])) 
				== directives_vect.end())
			{
				if (directive_components[0]  == "}" && directive_components.size() == 1)
					block_delem.second = true;
				return ;
			}
			checkDirectiveSyntax();
			(this->*direct_parsers[ptrdiff_t (it - directives_vect.begin())])();
		}
	}
}

ConfigParser::ConfigParser(const char *config_path, std::vector<Server> &servers) : _servers(servers)
{
	std::string			line;	
	
	config.open(config_path);
	if (!config.is_open())
		throw ConfigFileParsingException("failed to open ");
	directives_vect.assign(directives, directives + sizeof(directives) / sizeof(const char *));
	while (std::getline(config, line))
	{
		std::stringstream	ss(line);
		split(directive_components, ss);
		if (directive_components.empty())
			continue ;
		if (!checkServerSyntax())
			throw ConfigFileParsingException("invalid server block syntax");
		Server server;
		_servers.push_back(server);
		parseServerBlock();
		if (!block_delem.second)
			throw ConfigFileParsingException("invalid server block syntax");
	}
}


void	ConfigParser::parseServerName()
{
	_servers.back().server_name = directive_components;
}

void	ConfigParser::parseIndex()
{
	_servers.back().index = directive_components;		 
}

void	ConfigParser::parseRoot()
{
	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in root directive");
	_servers.back().root = directive_components.back();
}

void	ConfigParser::parseAutoIndex()
{
	std::string	value;

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

void	ConfigParser::parseClientMaxBodySize()
{
	std::vector<char>::iterator	it;
	std::string					value;

	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in client_max_body_size directive");
	value =  directive_components.back();
	it = std::find_if(value.begin(), value.end(), isalpha);
	if (it != value.end() && (!isunit(*it) || it != value.end() - 1))
			throw ConfigFileParsingException("invalid value in client_max_body_size directive");
	try
	{
		if (it != value.end())
			_servers.back().client_max_body_size = converttobytes(ft_stoll(value.c_str()), std::tolower(*it));
		else
			_servers.back().client_max_body_size = converttobytes(ft_stoll(value.c_str()), 'b');
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

	if (directive_components.size() < 2)
		throw ConfigFileParsingException("invalid number of arguments in error_page directive");
	try
	{
		it = directive_components.begin();
		for (; it != directive_components.end() - 1; it++)
		{
			code = ft_stoi(it->c_str());
			if (code < 300 || code > 600)
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

void	ConfigParser::parseListen()
{
	std::string	host, port;

	if (directive_components.size() > 1)
		throw ConfigFileParsingException("invalid number of arguments in listen directive");
	std::stringstream	ss(directive_components[0]);
	std::getline(ss, host, ':');
	ss >> port;
	if (host.empty() || port.empty() 
		|| !addr_resolver(&_servers.back().bind_addr, host.c_str(), port.c_str()))
		throw ConfigFileParsingException("invalid host or port in listen directive");
	// display_socket_addr(_servers.back().bind_addr);
}

void	ConfigParser::parseLocations()
{

}

