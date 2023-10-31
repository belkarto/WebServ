#include "ConfigParser.hpp"

void	ConfigParser::parseLocationBlock()
{
	std::string							line;
	std::vector<std::string>::iterator	it;
	Location							location;

	locat_directives.assign(location_direct , location_direct + NUM_LOCAT_DIREC);
	checkLocationSyntax();
	// whenever a location block is found => a location struct is defined and added to server's location in _servers
	location.root = _servers.back().root;
    location.index = _servers.back().index;
    location.autoindex = _servers.back().autoindex;
	_servers.back().location.push_back(location);
	// _servers.back().location.back() => the last location added in the last added server 
	_servers.back().location.back().uri = directive_components[1];
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
			(this->*location_parsers[std::ptrdiff_t (it - locat_directives.begin())])();
		}
	}
	if (!locat_block_delem.second)
			throw ConfigFileParsingException("invalid location block syntax");
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
	_servers.back().location.back().method = directive_components;
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
		_servers.back().location.back().redirect.first = code;
		_servers.back().location.back().redirect.second = directive_components[1];
	}
	catch (std::exception &e)
	{
		throw ConfigFileParsingException("invalid value in redirect directive");
	}
}

void	ConfigParser::parseLocationIndex()
{
	_servers.back().location.back().index = directive_components;		 
}

void	ConfigParser::parseLocationRoot()
{
	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in root directive");
	_servers.back().location.back().root = directive_components.back();
}

void	ConfigParser::parseLocationAutoIndex()
{
	std::string	value;

	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in autoindex directive");
	value = directive_components.back();
	if (value == "on")
		_servers.back().location.back().autoindex = true;
	else if (value == "off")
		_servers.back().location.back().autoindex = false;
	else
		throw ConfigFileParsingException("invalid value in autoindex directive");
}

void	ConfigParser::parseCgi()
{
	if (directive_components.size() != 2)
		throw ConfigFileParsingException("invalid number of arguments in cgi directive");
	_servers.back().location.back().cgi.first = directive_components[0];	// extension
	_servers.back().location.back().cgi.second = directive_components[1];	// interpreter
}

void	ConfigParser::parseUploadStore()
{
	if (directive_components.size() > 1)
		throw ConfigFileParsingException("invalid number of arguments in upload_store directive");
	_servers.back().location.back().upload_store = directive_components.back();
}
