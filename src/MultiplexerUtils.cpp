#include "Multiplexer.hpp"

void send_response_headers(Client &client) 
{

	client.sending = true;	// client can recieve response body after recieving headers
	std::stringstream	ss1(client.headers);
	ss1 >> client.path;
	client.path.clear();
	ss1 >> client.path;
	std::string	fullpath = "www" + client.path;
	// client.response = new std::ifstream("www/html/example.html");
	client.response = new std::ifstream(fullpath.c_str());

	// Get file size
	client.response->seekg(0, std::ios::end);
	client.response_size = client.response->tellg();
	client.response->seekg(0, std::ios::beg);

    // Convert the file length to a string
	std::stringstream ss;
	ss << client.response_size;
    std::string content_length = ss.str();

    // Construct the HTTP response with the file content
    std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: " + getMimeType(getFileExtension(fullpath)) + "\r\n"
                              "Connection: keep-alive\r\n"
                              "Content-Length: " + content_length + "\r\n"
                              "\r\n";

    // Send the rheaders to the client
    write(client.connect_socket, httpResponse.c_str(), httpResponse.size());
}

void	send_response_body(Client &client)
{
	int	count;

	client.response->read(client.buffer, MAX_SIZE);
	count = client.response->gcount();
	client.sent += write(client.connect_socket, client.buffer, count);
	// std::cout << client.sent << "     " << client.response_size << std::endl;
	if (client.sent == client.response_size)
	{
		std::cout << "all data is sent for " << client.connect_socket << std::endl;
		client.resetState();
		return ;
	}
}

