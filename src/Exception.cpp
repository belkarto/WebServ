#include "Exception.hpp"

const char*  ConfigFileParsingException::what() const throw()
{
    return error_message;
}

const char*  RequestParsingException::what() const throw()
{
    return status;
}