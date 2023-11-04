#include "Client.hpp"


Client::Client()
{
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