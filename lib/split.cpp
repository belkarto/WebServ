#include "webserv.hpp"

void    split(std::vector<std::string>& params, std::stringstream& ss)
{
    params.clear();
    std::string param;
    while (ss >> param)
        params.push_back(param);
    ss.clear();
    ss.str("");
}

// void    check_directive_syntax(std::vector<std::string>& params)
// {
//     if (params.empty())
//         throw Server::ConfigFileParsingException("invalid directive syntax");  
//     if (params.back() != ";" && params.back().back() == ';')
//     {
//         params.back().erase(params.back().end() - 1);
//         params.push_back(";");
//     }
//     if (params.back() != ";" || params.size() < 2)
//         throw Server::ConfigFileParsingException("invalid directive syntax");
//     params.pop_back();
// }