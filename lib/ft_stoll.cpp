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

void	safeConvert(long long &size, int factor)
{
	// make sure that size doesnt exceed the MAX and doesnt overflow
	size = (size > MAX_REQUEST_SIZE / factor) ? MAX_REQUEST_SIZE : size * factor;
}

long long	converttobytes(long long size, char unit)
{
	// using switch
	switch (unit)
	{
		case 'g':
			safeConvert(size, 1024);
			// fallthrough
		case 'm':
			safeConvert(size, 1024);
			// fallthrough
		case 'k':
			safeConvert(size, 1024);
			// fallthrough
	}
	return size;
}

int not_digit(char c) 
{
    return !std::isdigit(static_cast<unsigned char>(c));
}
