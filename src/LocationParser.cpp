#include "ConfigParser.hpp"

void	ConfigParser::parseLocationBlock()
{
	std::string							line;
	std::vector<std::string>::iterator	it;
	Location							location;

	locat_directives.assign(location_direct , location_direct + NUM_LOCAT_DIREC);
	checkLocationSyntax();
	_servers.back().emplaceBackLocation();
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
	_servers.back().location.back().method.clear();
	for (; it != directive_components.end(); it++)
	{
		if (std::find(methods.begin(), methods.end(), *it) == methods.end())
			throw ConfigFileParsingException("invalid value in method directive");
	}
	// remove multiple ocuurences
	std::sort(directive_components.begin(), directive_components.end());
	_servers.back().location.back().method.resize(directive_components.size());
	std::unique_copy(directive_components.begin(), directive_components.end()
		, _servers.back().location.back().method.begin());
}

void	ConfigParser::parseRedirect()
{
	if (directive_components.size() != 1)
		throw ConfigFileParsingException("invalid number of arguments in redirect directive");
	_servers.back().location.back().redirect = directive_components.back();
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
	_servers.back().location.back().cgi[directive_components[0]] = directive_components[1];	// extension [interpreter]
}

void	ConfigParser::parseUploadStore()
{
	if (directive_components.size() > 1)
		throw ConfigFileParsingException("invalid number of arguments in upload_store directive");
	_servers.back().location.back().upload_store = directive_components.back();
}
