// #include "WebServer.hpp"

// void    WebServer::readConfig(void)
// {
// 	const char	*arr[] = {
// 		"listen",
// 		"server_name",
// 		"root",
// 		"index",
// 		"client_body_max_size",
// 		"error_page",
// 		"location",
// 		"autoindex"
// 	};
// 	void (Server::*setdirectives[])(std::stringstream&) = {
// 		&Server::setListen,
// 		&Server::setServerName,
// 		&Server::setRoot,
// 		&Server::setIndex,
// 		&Server::setClientBodyMaxSize,
// 		&Server::setErrorPages,
// 		&Server::setLocations
// 	};
// 	std::vector<std::string>	directives(arr, arr +(sizeof(arr) / sizeof(char *)));
// 	std::ifstream   config(config_file);
// 	if (!config.is_open())
// 		throw std::ifstream::failure("failed to open config file"); // exception
// 	std::string line;
// 	while (std::getline(config, line))
// 	{
// 		std::stringstream		ss;
// 		std::string				directive, del, blank;
// 		std::pair<bool, bool>	blockdel;

// 		ss << line;
// 		ss >> directive;
// 		if (directive.empty())
// 			continue;
// 		ss >> del;
// 		ss >> blank;
// 		if ((directive != "server{" && directive != "server") 
// 				|| (del != "{" && !del.empty()) || !blank.empty())
// 			throw Server::ConfigFileParsingException("invalid server block syntax");
// 		if (directive == "server{")
// 			del = "{";
// 		blockdel.first = (del == "{") ? true : false;
// 		blockdel.second = false;
// 		Server server;
// 		while (std::getline(config, line))
// 		{
// 			ss.clear();
// 			ss.str("");
// 			directive.clear();
// 			del.clear();
// 			blank.clear();
// 			ss << line;
// 			ss >> directive;
// 			if (directive.empty())
// 				continue;
// 			if (!blockdel.first)
// 			{
// 				del = directive;
// 				ss >> blank;
// 				if (del != "{" || !blank.empty())
// 					throw Server::ConfigFileParsingException("invalid server block syntax");
// 				blockdel.first = true;
// 			}
// 			else
// 			{
// 				std::vector<std::string>::iterator	it;
// 				it = std::find(directives.begin(), directives.end(), directive);
// 				if (it == directives.end())
// 				{
// 					ss >> blank;
// 					if (directive != "}" || !blank.empty())
// 						throw Server::ConfigFileParsingException("invalid server block syntax");
// 					blockdel.second = true;
// 					break;
// 				}
// 				(server.*setdirectives[(ptrdiff_t) (it - directives.begin())])(ss);
// 			}
// 		}
// 		if (!blockdel.second)
// 			throw Server::ConfigFileParsingException("invalid server block syntax");
// 	}
// }