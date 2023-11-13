#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP

# include "webserv.hpp"

class ConfigFileParsingException : public  std::exception
{
	private:
		const char	*error_message;
	public:
		ConfigFileParsingException(const char *error_message)
		{
			this->error_message = error_message;
		}
		virtual const char* what() const throw();
};

class RequestParsingException : public  std::exception
{
	private:
		const char	*status;
	public:
		RequestParsingException(const char *status)
		{
			this->status = status;
		}
		virtual const char* what() const throw();
};
#endif
