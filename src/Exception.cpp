#include "Exception.hpp"

const char*  ConfigFileParsingException::what() const throw()
{
    return error_message;
}