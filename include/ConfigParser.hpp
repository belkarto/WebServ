#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "webserv.hpp"
# include "Server.hpp"

# define NUM_DIREC 8

class ConfigParser
{
	std::ifstream					config;
	std::vector<std::string>		directives_vect;
	std::vector<std::string>		directive_components;
	std::pair<bool, bool>			block_delem;
	std::vector<Server> 			&_servers;

    public:
		ConfigParser(const char *config_path, std::vector<Server> &servers);

		bool		checkServerSyntax();
		void		checkDirectiveSyntax();
		void		parseServerBlock();
		void		parseListen();
		void		parseServerName();
		void		parseRoot();
		void		parseIndex();
		void		parseClientMaxBodySize();
		void		parseErrorPages();
		void		parseLocations();
		void		parseAutoIndex();

		static const char			*directives[NUM_DIREC];
		static void					(ConfigParser::*direct_parsers[NUM_DIREC])(void);
};
#endif