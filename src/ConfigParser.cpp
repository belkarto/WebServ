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

void (*ConfigParser::direct_parsers[NUM_DIREC])(void) = {
    &ConfigParser::parseListen,
    &ConfigParser::parseServerName,
    &ConfigParser::parseRoot,
    &ConfigParser::parseIndex,
    &ConfigParser::parseClientMaxBodySize,
    &ConfigParser::parseErrorPages,
    &ConfigParser::parseLocations,
    &ConfigParser::parseAutoIndex,
};

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
			else
				(*direct_parsers[(ptrdiff_t) (it - directives_vect.begin())])();
		}
	}
}

ConfigParser::ConfigParser(const char *config_path)
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
		parseServerBlock();
		if (!block_delem.second)
			throw ConfigFileParsingException("invalid server block syntax");
	}
}


void	ConfigParser::parseListen()
{

}

void	ConfigParser::parseServerName()
{

}

void	ConfigParser::parseRoot()
{

}

void	ConfigParser::parseIndex()
{

}

void	ConfigParser::parseClientMaxBodySize()
{

}

void	ConfigParser::parseErrorPages()
{

}

void	ConfigParser::parseLocations()
{

}

void	ConfigParser::parseAutoIndex()
{

}
