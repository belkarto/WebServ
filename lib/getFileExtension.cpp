#include "webserv.hpp"

std::string getFileExtension(const std::string& fileName) 
{
    size_t dotPosition = fileName.find_last_of('.');
    if (dotPosition != std::string::npos)
        return fileName.substr(dotPosition);
    return "";
}