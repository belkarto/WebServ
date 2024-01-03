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

void    setHeader(std::string &headers, std::string key, std::string value)
{
    headers += key + value + CRLF;
}

void Response::sendHeaders(CLIENTIT &clientIt)
{
    std::string headers;

    clientIt->start_responding = true;
    if (cgi)
        parseCgi();
    headers = "HTTP/1.1 ";
    headers += status + CRLF;
    if (!contentType.empty())
        setHeader(headers, "Content-Type: ", contentType);
    if (!transferEncoding.empty())
        setHeader(headers, "Transfer-Encoding: ", transferEncoding);
    else if (!contentLength.empty())
        setHeader(headers, "Content-Length: ", contentLength);
    if (!location.empty())
        setHeader(headers, "Location: ", location);
    connection = clientIt->fields["Connection"];
    if (connection.empty())
        connection = "close";
    setHeader(headers, "Connection: ", connection);
    if (status == STATUS_201 && !fileLocation.empty())
        setHeader(headers, "Content-Location: ", fileLocation);
    if (clientIt->fields["Cookie"].empty())
        setHeader(headers, "Set-Cookie: ", cookieGenerator(clientIt));
    else
        setHeader(headers, "Cookie: " ,clientIt->fields["Cookie"]);
    while (!cookies.empty())
    {
        setHeader(headers, "Set-Cookie: ", cookies.back());
        cookies.pop_back();
    }
    headers += CRLF;
    if (send(clientIt->connect_socket, &headers[0], headers.length(), 0) < 1)
        throw ResponseSendingException("ResponseSendingException");
}

void    Response::parseCgi(void)
{
    std::string     line, key, value;
    bool            found;
    size_t          pos;

    found = false;
    contentType = "text/html";
    while(std::getline(*fileContent, line))
    {
        if (line == "\r")
            break;
        if ((pos = line.find(':')) != std::string::npos)
        {
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
            std::transform(key.begin(), key.end(), key.begin(), tolower);
        }
        if (key == "status")
        {
            found = true;
            status = value;
        }
        if (key == "location")
        {
            found = true;
            location = value;
        }
        if (key == "content-type")
        {
            found = true;
            contentType = value;
        }
        if (key == "set-cookie")
        {
            found = true;
            cookies.push_back(value);
        }
    }
    if (!found)
    {
        fileContent->clear();
        fileContent->seekg(0, std::ios::beg);
    }
    else
    {
        std::stringstream   ss(contentLength);
        std::streamsize size;
        ss >> size;
        response_size = size - fileContent->tellg();
        contentLength = toString(response_size);
    }
}

void    Response::parseCgi(void)
{
    std::string     line, key, value;
    bool            found;
    size_t          pos;

    found = false;
    contentType = "text/html";
    while(std::getline(*fileContent, line))
    {
        if (line == "\r")
            break;
        if ((pos = line.find(':')) != std::string::npos)
        {
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
            std::transform(key.begin(), key.end(), key.begin(), tolower);
        }
        if (key == "status")
        {
            found = true;
            status = value;
        }
        if (key == "location")
        {
            found = true;
            location = value;
        }
        if (key == "content-type")
        {
            found = true;
            contentType = value;
        }
        if (key == "set-cookie")
        {
            found = true;
            cookies.push_back(value);
        }
    }
    if (!found)
    {
        fileContent->clear();
        fileContent->seekg(0, std::ios::beg);
    }
    else
    {
        std::stringstream   ss(contentLength);
        std::streamsize size;
        ss >> size;
        response_size = size - fileContent->tellg();
        contentLength = toString(response_size);
    }
}

void Response::sendResponseBuffer(CLIENTIT &clientIt)
{
    char            buffer[CLIENT_RESPONSE_BUFFER_SIZE];
    std::streamsize rd;

    if (!transferEncoding.empty())
            return (sendAutoIndexBuffer(clientIt));
    if (readbytes < response_size)
    {
        fileContent->read(buffer, CLIENT_RESPONSE_BUFFER_SIZE);
        rd = fileContent->gcount();
        readbytes += rd;
        if (send(clientIt->connect_socket, &buffer, rd, 0) < 1)
            throw ResponseSendingException("ResponseSendingException");
    }
    if (readbytes == response_size)
    {
        fileContent->close();
        delete fileContent;
        fileContent = NULL;
        unlink(CgiFilePath.c_str());
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
    if (send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0) < 1 
        || send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0) < 1)
        throw ResponseSendingException("ResponseSendingException");
    if (!entry)
    {
        chunk_data = CRLF;
        chunk_size = "0" + chunk_data;
        if (send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0) < 1 
        || send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0) < 1)
        throw ResponseSendingException("ResponseSendingException");
        closedir(directory);
        directory = NULL;
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
