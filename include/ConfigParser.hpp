#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "webserv.hpp"
# include "Server.hpp"

# define NUM_SERV_DIREC 8
# define NUM_LOCAT_DIREC 5

class ConfigParser
{
	std::ifstream					config;
	std::vector<std::string>		directive_components;	// directive line is splited and stored here
	std::vector<std::string>		server_directives;
	std::vector<std::string>		locat_directives;
	std::pair<bool, bool>			block_delem;			// used to check if both { and } were found
	std::pair<bool, bool>			locat_block_delem;
	std::vector<Server> 			&_servers;				// gets filled with each server found in config file

    public:
		ConfigParser(const char *config_path, std::vector<Server> &servers);

		void		checkDirectiveSyntax();

		// server context
		void		parseServerBlock();
		bool		checkServerSyntax();
		void		parseListen();
		void		parseServerName();
		void		parseClientMaxBodySize();
		void		parseErrorPages();
		void		parseRoot();
		void		parseIndex();
		void		parseAutoIndex();

		// location context
		void		parseLocationBlock();
		void		checkLocationSyntax();
		void		parseLocationRoot();
		void		parseLocationIndex();
		void		parseLocationAutoIndex();
		void		parseMethod();
		void		parseRedirect();

		// server directives and their corresponding functions to parse them
		static const char			*server_direct[NUM_SERV_DIREC];
		static const char			*location_direct[NUM_LOCAT_DIREC];
		static void					(ConfigParser::*server_parsers[NUM_SERV_DIREC])(void);
		static void					(ConfigParser::*location_parsers[NUM_LOCAT_DIREC])(void);
};
#endif
