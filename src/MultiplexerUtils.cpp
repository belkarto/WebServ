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
	if (!client.response->is_open())
	{
		std::cout << "failed to open file" << std::endl;
		client.sending = false;
		client.recieved = 0;
		client.sent = 0;
		client.response_size = 0;
		client.path.clear();
		client.headers.clear();
		delete client.response;
		std::string response_body = "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>";
		std::stringstream	ss;
		ss << response_body.length();
		std::string content_length = ss.str();
		std::string httpResponse = "HTTP/1.1 404 Not Found\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: " + content_length + "\r\n"
                          "\r\n" + response_body;

		write(client.connect_socket, httpResponse.c_str(), httpResponse.size());
		return ;
	}
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
		client.last_activity = time(NULL);
		client.response->close();
		client.resetState();
		return ;
	}
}

