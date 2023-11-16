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

void    split(std::vector<std::string>& params, std::string str)
{
    std::stringstream   ss;
    std::string         param;

    ss << str;
    params.clear();
    while (ss >> param)
        params.push_back(param);
}