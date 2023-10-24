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
