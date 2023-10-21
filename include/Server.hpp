#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

typedef struct location
{
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::vector<std::string>	allowed_methods;
	std::pair<int, std::string>	redirection;
}location;

class Server
{
	public:
		Server(void);
		struct addrinfo								*server_addr;
		std::vector<std::string>					server_name;
		size_t										client_body_max_size;
		std::string									root;
		std::vector<std::string>					index;
		std::map<std::vector<int>, std::string >	error_pages;
		std::vector<struct location>				locations;

		void	setListen(std::stringstream& ss);
		void	setServerName(std::stringstream& ss);
		void	setClientBodyMaxSize(std::stringstream& ss);
		void	setRoot(std::stringstream& ss);
		void	setIndex(std::stringstream& ss);
		void	setErrorPages(std::stringstream& ss);
		void	setLocations(std::stringstream& ss);

		class ConfigFileParsingException : public  std::exception
		{
			private:
				const char	*errorMessage;
			public:
				ConfigFileParsingException(const char* errorMessage)
				{
					this->errorMessage = errorMessage;
				}
				virtual const char* what() const throw();
		};
};
#endif
