#include "Server.hpp"

Server::Server()
{

}

void	Server::setListen(std::stringstream& ss)
{
	std::string	addr, port, del;
	std::getline(ss, addr, ':');
	std::getline(ss, port, ';');
	ss >> del;
	std::cout << "|" << addr << "|" << port << "|" << std::endl;
	struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
	int status;
    if ((status = getaddrinfo("127.0.0.1", "80", &hints, &result)) != 0)
		std::cerr << "Error: " << gai_strerror(status) << std::endl;
	freeaddrinfo(result);
}

void	Server::setServerName(std::stringstream& ss)
{

}

void	Server::setClientBodyMaxSize(std::stringstream& ss)
{

}

void	Server::setRoot(std::stringstream& ss)
{

}

void	Server::setIndex(std::stringstream& ss)
{

}

void	Server::setErrorPages(std::stringstream& ss)
{

}

void	Server::setLocations(std::stringstream& ss)
{

}

const char* Server::ConfigFileParsingException::what() const throw()
{
    return errorMessage;
}