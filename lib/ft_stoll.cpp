#include "webserv.hpp"

long long	ft_stoll(const char* str)
{
    char		*endptr;
    long long	value;

	errno = 0;
	value = strtoll(str, &endptr, 10);
	if (errno == ERANGE)
	    throw std::out_of_range(str);
	if (str == endptr)
	    throw std::invalid_argument(str);
	return value;
}

int	isunit(int unit)
{
	unit = tolower(unit);
	return (unit == 'k' || unit == 'm' || unit == 'g');
}

long long	converttobytes(long long size, char unit)
{
	//	TODO:	
	//	check size if biger than max request size and fix it
	switch (unit)
	{
		case 'g':
			size *= 1024;
			// fallthrough
		case 'm':
			size *= 1024;
			// fallthrough
		case 'k':
			size *= 1024;
			// fallthrough
	}
	return size;
}