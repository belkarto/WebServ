// #include "Server.hpp"

// Server::Server()
// {

// }

// void	Server::setListen(std::stringstream& ss)
// {
//     std::string s, addr, port, blank;
//     getline(ss, s);
//     std::string::iterator it = std::remove(s.begin(), s.end(), ' ');
//     s.erase(it, s.end());
// 	if (s.find(':') == std::string::npos || s.find(';') == std::string::npos)
//         throw ConfigFileParsingException("invalid directive syntax");
//     ss.clear();
//     ss.str("");
//     ss << s;
//     getline(ss, addr, ':');
//     getline(ss, port, ';');
//     ss >> blank;
//     if (!blank.empty())
//         throw ConfigFileParsingException("invalid directive syntax");
// 	addr_resolver(&(this->server_addr), addr, port);
// }

// void	Server::setServerName(std::stringstream& ss)
// {
//     split(server_name, ss);
// 	check_directive_syntax(server_name);     
// }

// void	Server::setClientBodyMaxSize(std::stringstream& ss)
// {
//     std::vector<std::string>    params;
//     char                unit;

//     split(params, ss);
//     check_directive_syntax(params);
//     if (params.size() > 1)
//         throw ConfigFileParsingException("invalid directive syntax");
//     unit = 'b';
//     try
//     {
//         unit = params[0].back();
//         if (!isdigit(unit) && unit != 'k' && unit != 'm' && unit != 'g')
//             throw ConfigFileParsingException("invalid directive params");           
//         client_body_max_size = convert_to_bytes(ft_stoul(params[0].c_str()), unit);
//     }
//     catch (std::exception& e)
//     {
//         throw ConfigFileParsingException("invalid directive params");
//     }
// }

// void	Server::setRoot(std::stringstream& ss)
// {
//     std::vector<std::string>    params;

//     split(params, ss);
//     check_directive_syntax(params);
//     if (params.size() > 1)
//         throw ConfigFileParsingException("invalid directive syntax");
//     root = params[0];
// }

// void	Server::setIndex(std::stringstream& ss)
// {
//     split(index, ss);
// 	check_directive_syntax(index);
// }

// void	Server::setErrorPages(std::stringstream& ss)
// {
// 	std::vector<std::string>					params;
// 	const char*									str;
//     char										*endptr;
//     long										value;
// 	std::pair<std::vector<int>, std::string>	elem;

// 	split(params, ss);
// 	check_directive_syntax(params);
// 	if (params.size() < 2)
// 	    throw ConfigFileParsingException("invalid directive syntax");
// 	elem.second = params.back();
// 	params.pop_back();
// 	std::vector<std::string>::iterator it = params.begin();
// 	for (; it != params.end(); it++)
// 	{
// 		str = it->c_str();
// 		errno = 0;
// 		value = strtoul(str, &endptr, 10);
// 		if (errno == ERANGE || str == endptr || *endptr)
// 		    throw ConfigFileParsingException("invalid directive params");
// 		elem.first.push_back(value);
// 	}
// }

// void	Server::setLocations(std::stringstream& ss)
// {

// }

// const char* Server::ConfigFileParsingException::what() const throw()
// {
//     return errorMessage;
// }