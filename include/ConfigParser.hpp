#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "webserv.hpp"

# define NUM_DIREC 8

class ConfigParser
{
	std::ifstream					config;
	std::vector<std::string>		directives_vect;
	std::vector<std::string>		directive_components;
	std::pair<bool, bool>			block_delem;

    public:
		ConfigParser(const char *config_path);
		bool		checkServerSyntax();
		void		parseServerBlock();
		
		static void		parseListen();
		static void		parseServerName();
		static void		parseRoot();
		static void		parseIndex();
		static void		parseClientMaxBodySize();
		static void		parseErrorPages();
		static void		parseLocations();
		static void		parseAutoIndex();

		static const char				*directives[NUM_DIREC];
		static void						(*direct_parsers[NUM_DIREC])(void);
};
#endif