#include "../include/Multiplexer.hpp"

static std::string cookieGenerator(CLIENTIT &clientIt)
{
    (void)clientIt;
    std::string cookie;
    std::time_t currentTime;
    std::stringstream ss;

    cookie = "user_id=";
    currentTime = std::time(NULL);


    int randomUserId = std::rand();

    ss << randomUserId << "_" << currentTime;

    // Append current time to the user_id
    ss << "_" << currentTime;

    cookie += ss.str();
    return cookie;
}

void Response::sendHeaders(CLIENTIT &clientIt)
{
    std::string headers;

    clientIt->start_responding = true;
    headers = "HTTP/1.1 ";
    headers += status + CRLF;
    if (cgi)
    {
        contentType = "text/html";
    }
    if (!contentType.empty())
        headers += "Content-Type: " + contentType + CRLF;
    if (!transferEncoding.empty())
        headers += "Transfer-Encoding: " + transferEncoding + CRLF;
    else if (!contentLength.empty())
        headers += "Content-Length: " + contentLength + CRLF;
    if (!location.empty())
        headers += "Location: " + location + CRLF;
    connection = clientIt->fields["Connection"];
    if (connection.empty())
        connection = "close";
    headers += "Connection: " + connection + CRLF;
    if (status == STATUS_201 && !fileLocation.empty())
        headers += "Content-Location: " + fileLocation + CRLF;
    if (clientIt->fields["Cookie"].empty())
        headers += "Set-Cookie: " + cookieGenerator(clientIt) + CRLF;
    else
        headers += "Cookie: " + clientIt->fields["Cookie"] + CRLF;
    headers += CRLF;
    std::cout << RED << "Response headers: " << headers << RESET << std::endl;
    send(clientIt->connect_socket, &headers[0], headers.length(), 0);
}

void Response::sendResponseBuffer(CLIENTIT &clientIt)
{
    char            buffer[CLIENT_RESPONSE_BUFFER_SIZE];
    std::streamsize rd;

    if (!transferEncoding.empty())
    {
        if (!cgi)
            return (sendAutoIndexBuffer(clientIt));
        return (sendPipeBuffer(clientIt));
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
        if (readbytes == response_size)
        {
            fileContent->close();
            delete fileContent;
            clientIt->response_all_sent = true;
        }
    }
}

void Response::sendPipeBuffer(CLIENTIT &clientIt)
{
    char              buffer[CLIENT_RESPONSE_BUFFER_SIZE];
    std::streamsize   rd;
    std::string       chunk_size;
    std::string       chunk_data;
    std::stringstream ss;

    if ((rd = read(fd, buffer, CLIENT_RESPONSE_BUFFER_SIZE)) > 0)
    {
        chunk_data = "";
        chunk_data.append(buffer, rd);
        ss << std::hex << chunk_data.length();
        ss >> chunk_size;
        chunk_data += CRLF;
        chunk_size += CRLF;
        send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0);
        send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0);
    }
    else
    {
        chunk_data = CRLF;
        chunk_size = "0" + chunk_data;
        send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0);
        send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0);
        close(fd);
        clientIt->response_all_sent = true;
    }
}

void Response::sendAutoIndexBuffer(CLIENTIT &clientIt)
{
    struct dirent    *entry;
    std::string       chunk_data;
    std::string       chunk_size;
    std::stringstream ss;

    entry = readdir(directory);
    chunk_data = "";
    if (entry && !strcmp(entry->d_name, "."))
        chunk_data = "<html>\n<head>\n<title>Index of " + filePath + "</title>\n</head>\n<body>\n<h1>Index of " +
                     filePath + "</h1>\n<hr>\n<pre>\r\n";
    while (entry && chunk_data.length() != CLIENT_RESPONSE_BUFFER_SIZE)
    {
        chunk_data.append("<a href=\"");
        chunk_data.append(clientIt->fields[URI]);
        if ((clientIt->fields[URI])[clientIt->fields[URI].length() - 1] != '/')
            chunk_data.append("/");
        chunk_data.append(entry->d_name);
        chunk_data.append("\">");
        chunk_data.append(entry->d_name);
        chunk_data.append("</a>\n");
        entry = readdir(directory);
    }
    if (!entry)
        chunk_data.append("</pre>\n<hr>\n</body>\n</html>");
    ss << std::hex << chunk_data.length();
    ss >> chunk_size;
    chunk_data += CRLF;
    chunk_size += CRLF;
    send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0);
    send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0);
    if (!entry)
    {
        chunk_data = CRLF;
        chunk_size = "0" + chunk_data;
        send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0);
        send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0);
        closedir(directory);
        clientIt->response_all_sent = true;
    }
}

std::string Response::getErrorPage(int errorCode)
{
    std::string                          finalResponse;
    std::map<int, std::string>::iterator it;

    it = Multiplexer::defErrorPages.find(errorCode);
    finalResponse = "<html>" CRLF "<head><title>";
    finalResponse.append(it->second)
        .append("</title></head>" CRLF "<body>" CRLF "<center><h1>")
        .append(it->second)
        .append("</h1></center>" CRLF)
        .append("</body></html>" CRLF);
    return finalResponse;
}
