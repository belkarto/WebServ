#include "Client.hpp"


Client::Client(int sockfd, int listen_socket)
{
	this->sockfd = sockfd;
	this->listen_socket = listen_socket;
	this->recieved = 0;
	this->sent = 0;
	this->response = NULL;
	this->response_size = 0;
	this->headers = "";
	this->sending = false;
	this->path = "";
}

void    Client::resetState()
{
	delete this->response;
	this->recieved = 0;
	this->sent = 0;
	this->response = NULL;
	this->response_size = 0;
	this->headers.clear();
	this->sending = false;
	this->path = "";
}