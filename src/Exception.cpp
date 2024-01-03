#include "Exception.hpp"

const char*  ConfigFileParsingException::what() const throw()
{
    return error_message;
}

const char*  RequestParsingException::what() const throw()
{
    return status;
}

const char*   ResponseSendingException::what() const throw()
{
    return status;
}