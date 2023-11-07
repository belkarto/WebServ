#include "Multiplexer.hpp"

void send_response(std::vector<Client>::iterator &clientIt)
{
    std::string response = "HTTP/1.1 200 OK\n";
    response += "Content-Type: text/html\n";

    // Calculate the length of the response body
    std::string htmlBody = "<!DOCTYPE html>\n<html>\n<body>\n<h1>Hello World!</h1>\n</body>\n</html>\n\n\n";
    int contentLength = htmlBody.size();

    // Add the Content-Length header
    response += "Content-Length: " + std::to_string(contentLength) + "\n";

    response += "\n";
    response += htmlBody;

    write(clientIt->connect_socket, response.c_str(), response.size());
	clientIt->response_all_sent = true;
}