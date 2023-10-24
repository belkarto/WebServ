#include "webserv.hpp"

int ft_stoi(const char* str)
{
    char		*endptr;
    long		value;

	errno = 0;
	value = strtol(str, &endptr, 10);
	if (errno == ERANGE || value > INT_MAX || value < INT_MIN)
	    throw std::out_of_range(str);
	if (str == endptr || *endptr)
	    throw std::invalid_argument(str);
	return value;
}