#include "Multiplexer.hpp"

void	Response::generateResponse(CLIENTIT& clientIt)
{
    std::cout << __FUNCTION__ << std::endl;
	std::stringstream	ss;

	clientIt->start_responding = true;
    if (!special_response.empty())
        response_size = special_response.length();
    if (transferEncoding.empty())
    {
        fileContent = new std::ifstream(filePath.c_str());
        response_size = getFileSize(fileContent);
        contentType = clientIt->getMimeType(filePath);
    }
    ss << response_size;
    ss >> contentLength;
	sendHeaders(clientIt);
}

void	Response::sendHeaders(CLIENTIT& clientIt)
{
    std::cout << __FUNCTION__ << std::endl;
	std::string	headers;

	headers = "";
	headers += "HTTP/1.1 ";
	headers += status + CRLF;
	headers += "Content-Type: " + contentType + CRLF;
    if (transferEncoding.empty())
	    headers += "Content-Length: " + contentLength + CRLF;
    else
	{
        headers += "Transfer-Encoding: " + transferEncoding + CRLF;
	}
	if (!location.empty())
		headers += "Location: " + location + CRLF;
	headers += "Connection: " + connection + CRLF;
	headers += CRLF;
	send(clientIt->connect_socket, headers.c_str(), headers.length(), 0);
    if (!special_response.empty())
    {
        clientIt->response_all_sent = true;
        send(clientIt->connect_socket, &special_response[0], special_response.length(), 0);
    }
}

void	Response::sendResponseBuffer(CLIENTIT& clientIt)
{
    std::cout << __FUNCTION__ << std::endl;

	char	 		    buffer[CLIENT_RESPONSE_BUFFER_SIZE];
	std::streamsize	    rd;
    struct dirent       *entry;
    std::string         chunk_data;
    std::string         chunk_size;
    std::stringstream   ss;
	std::string			delim;

	chunk_data = "";
    if (!transferEncoding.empty())
    {
		chunk_data = "<html>\n<head>\n<title>Index of " + filePath
                        + "</title>\n</head>\n<body>\n<h1>Index of " + filePath
                        + "</h1>\n<hr>\n<pre>\r\n";
        while ((entry = readdir(directory)) != NULL)
        {
            chunk_data.append("<a href=\"");
            chunk_data.append(entry->d_name);
            chunk_data.append("\">");
			chunk_data.append(entry->d_name);
			chunk_data.append("</a>\n");
        }
		chunk_data.append("</pre>\n<hr>\n</body>\n</html>");
        ss << std::hex << chunk_data.length();
		ss >> chunk_size;
		send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0);
		send(clientIt->connect_socket, CRLF, 2, 0);
		send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0);
		send(clientIt->connect_socket, CRLF, 2, 0);
		/**/
		chunk_data = "";
		chunk_size = "0";
		send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0);
		send(clientIt->connect_socket, CRLF, 2, 0);
		send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0);
		send(clientIt->connect_socket, CRLF, 2, 0);
		clientIt->response_all_sent = true;
    }
    else
    {
        if (readbytes < response_size)
        {
            fileContent->read(buffer, CLIENT_RESPONSE_BUFFER_SIZE);
            rd = fileContent->gcount();
            readbytes += rd;
            send(clientIt->connect_socket, &buffer, rd, 0);
        }
        else
            clientIt->response_all_sent = true;
    }
}

std::string Response::getErrorPage(int errorCode) 
{
    std::string                           finalResponse;
    std::map<int, std::string>::iterator it;
  
    it = Multiplexer::defErrorPages.find(errorCode);
    finalResponse = "<html>" CRLF "<head><title>";
    finalResponse.append(it->second)
                  .append("</title></head>" CRLF "<body>" CRLF "<center><h1>")
                  .append(it->second)
                  .append("</h1></center>" CRLF);   
    return finalResponse;
}
