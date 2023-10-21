#include "webserv.hpp"

size_t	ft_stoul(const char* str)
{
    char	*endptr;
    size_t		value;

	errno = 0;
	value = strtoul(str, &endptr, 10);
	if (errno == ERANGE)
	    throw std::out_of_range(str);
	if (str == endptr)
	    throw std::invalid_argument(str);
	return value;
}

size_t	convert_to_bytes(size_t size, char unit)
{
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