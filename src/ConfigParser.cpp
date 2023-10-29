#include "ConfigParser.hpp"

const char	*ConfigParser::directives[NUM_DIREC] = {
	"listen",
	"server_name",
	"client_max_body_size",
	"error_page",
	"autoindex",
	"location",
	"root",
	"index",
	"method",
	"redirect",
};

void (ConfigParser::*ConfigParser::direct_parsers[NUM_DIREC])(void) = {
    &ConfigParser::parseListen,
    &ConfigParser::parseServerName,
    &ConfigParser::parseClientMaxBodySize,
    &ConfigParser::parseErrorPages,
    &ConfigParser::parseAutoIndex,
    &ConfigParser::parseLocations,
    &ConfigParser::parseRoot,
    &ConfigParser::parseIndex,
	&ConfigParser::parseMethod,
	&ConfigParser::parseRedirect,
};

void     ConfigParser::checkDirectiveSyntax()
{
	char	del;

	del = directive_components.back()[directive_components.back().length() - 1];
    if (directive_components.back() != ";" && del == ';')
    {
        directive_components.back().erase(directive_components.back().end() - 1);
        directive_components.push_back(";");
    }
    if (directive_components.back() != ";" || directive_components.size() < 3)
	{

        throw ConfigFileParsingException("invalid directive syntax");
	}
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
			if ((it = std::find(server_directives.begin(), server_directives.end(), directive_components[0])) 
				== server_directives.end())
			{
				if (directive_components[0]  == "}" && directive_components.size() == 1)
					block_delem.second = true;
				else
					throw ConfigFileParsingException("invalid directive name");
				return ;
			}
			if (directive_components[0] != "location")
				checkDirectiveSyntax();
			(this->*direct_parsers[std::ptrdiff_t (it - server_directives.begin())])();
		}
	}
}

ConfigParser::ConfigParser(const char *config_path, std::vector<Server> &servers) : _servers(servers)
{
	std::string			line;	
	
	config.open(config_path);
	if (!config.is_open())
		throw ConfigFileParsingException("failed to open ");
	server_directives.assign(directives, directives + (NUM_DIREC - 2));
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
	std::string::iterator	it;
	std::string				value;

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
	display_socket_addr(_servers.back().bind_addr);
}

void	ConfigParser::checkLocationSyntax()
{
	locat_block_delem.first = false;
	locat_block_delem.second = false;
	if (directive_components.size() < 2 || directive_components.size() > 3)
		throw ConfigFileParsingException("invalid location block syntax");
	if (directive_components.size() == 3)
	{
		if (directive_components[2] != "{")
			throw ConfigFileParsingException("invalid location block syntax");
		else
			locat_block_delem.first = true;
	}
}

void	print(std::string str)
{
	std::cout << str << std::endl;
}

void	ConfigParser::parseMethod()
{
	std::vector<std::string>			methods;
	std::vector<std::string>::iterator	it;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	it = directive_components.begin();
	for (; it != directive_components.end(); it++)
	{
		if (std::find(methods.begin(), methods.end(), *it) == methods.end())
			throw ConfigFileParsingException("invalid value in method directive");
	}
	_servers.back().location.method = directive_components;
}

void	ConfigParser::parseRedirect()
{
	int code;

	if (directive_components.size() != 2)
		throw ConfigFileParsingException("invalid number of arguments in redirect directive");
	try
	{
		code = ft_stoi(directive_components[0].c_str());
		// redirect code falls in 3xx range
		if (code >= 400 || code < 300)
			throw ConfigFileParsingException("invalid value in redirect directive");
		_servers.back().location.redirect.first = code;
		_servers.back().location.redirect.second = directive_components[1];
	}
	catch (std::exception &e)
	{
		throw ConfigFileParsingException("invalid value in redirect directive");
	}
}

void	ConfigParser::parseLocations()
{
	std::string							line;
	std::vector<std::string>::iterator	it;

	locat_directives.assign(directives + 6, directives + NUM_DIREC);
	checkLocationSyntax();
	_servers.back().location.uri = directive_components[1];
	while(getline(config, line))
	{
		std::stringstream	ss(line);
		split(directive_components, ss);
		if (directive_components.empty())
			continue;
		if (!locat_block_delem.first)
		{
			if (directive_components[0] != "{" || directive_components.size() > 1)
				throw ConfigFileParsingException("invalid location block syntax");	
			locat_block_delem.first = true;	
		}
		else
		{
			if ((it = std::find(locat_directives.begin(), locat_directives.end(), directive_components[0])) 
				== locat_directives.end())
			{
				if (directive_components[0]  == "}" && directive_components.size() == 1)
					locat_block_delem.second = true;
				else
					throw ConfigFileParsingException("invalid directive name");
				return ;
			}
			checkDirectiveSyntax();
			(this->*direct_parsers[std::ptrdiff_t (it - locat_directives.begin()) + 6])();
		}
	}
	if (!locat_block_delem.second)
			throw ConfigFileParsingException("invalid location block syntax");
}

