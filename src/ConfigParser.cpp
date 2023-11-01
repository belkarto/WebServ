#include "ConfigParser.hpp"

const char	*ConfigParser::server_direct[NUM_SERV_DIREC] = {
	"listen",
	"server_name",
	"client_max_body_size",
	"error_page",
	"location",
	"autoindex",
	"root",
	"index",
};

const char	*ConfigParser::location_direct[NUM_LOCAT_DIREC] = {
	"autoindex",
	"root",
	"index",
	"method",
	"redirect",
	"cgi",
	"upload_store",
};

void (ConfigParser::*ConfigParser::server_parsers[NUM_SERV_DIREC])(void) = {
    &ConfigParser::parseListen,
    &ConfigParser::parseServerName,
    &ConfigParser::parseClientMaxBodySize,
    &ConfigParser::parseErrorPages,
    &ConfigParser::parseLocationBlock,
    &ConfigParser::parseAutoIndex,
    &ConfigParser::parseRoot,
    &ConfigParser::parseIndex,
};

void (ConfigParser::*ConfigParser::location_parsers[NUM_LOCAT_DIREC])(void) = {
    &ConfigParser::parseLocationAutoIndex,
    &ConfigParser::parseLocationRoot,
    &ConfigParser::parseLocationIndex,
	&ConfigParser::parseMethod,
	&ConfigParser::parseRedirect,
	&ConfigParser::parseCgi,
	&ConfigParser::parseUploadStore,
};

// each directive should end with ; and have at least one arg
void     ConfigParser::checkDirectiveSyntax()
{
	char	del;

	del = directive_components.back()[directive_components.back().length() - 1];
    if (directive_components.back() != ";" && del == ';')
    {
		// in case we have the following syntax
		// directive arg1 arg2;
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


ConfigParser::ConfigParser(const char *config_path, std::vector<Server> &servers) : _servers(servers)
{
	std::string			line;	
	
	config.open(config_path);
	if (!config.is_open())
		throw ConfigFileParsingException("failed to open ");
	server_directives.assign(server_direct, server_direct + NUM_SERV_DIREC);
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
	if (_servers.empty())
		throw ConfigFileParsingException("no configuration provided in config file");
}









