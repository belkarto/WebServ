#include "../include/Multiplexer.hpp"

static std::string cookieGenerator(void)
{
    std::string       cookie;
    std::time_t       currentTime;
    std::stringstream ss;
    int               randomUserId;

    cookie = "user_id=";
    currentTime = std::time(NULL);
    randomUserId = std::rand();
    ss << randomUserId << "_" << currentTime;
    cookie += ss.str();
    return cookie;
}

void setHeader(std::string &headers, std::string key, std::string value)
{
    headers += key + value + CRLF;
}

void Response::sendHeaders(CLIENTIT &clientIt)
{
    std::string headers;

    clientIt->start_responding = true;
    if (cgi)
        parseCgi();
    headers = PROTOCOL_VERS;
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
        setHeader(headers, "Set-Cookie: ", cookieGenerator());
    else
        setHeader(headers, "Cookie: ", clientIt->fields["Cookie"]);
    setHeader(headers, "Server: ", SERVER);
    while (!cookies.empty())
    {
        setHeader(headers, "Set-Cookie: ", cookies.back());
        cookies.pop_back();
    }
    headers += CRLF;
    if (send(clientIt->connect_socket, &headers[0], headers.length(), 0) < 1)
        throw ResponseSendingException("ResponseSendingException");
}

void Response::parseCgi(void)
{
    std::string line, key, value;
    bool        found;
    size_t      pos;

    found = false;
    contentType = "text/html";
    while (std::getline(*fileContent, line))
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
        std::stringstream ss(contentLength);
        std::streamsize   size;
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

static std::string formatDateTime(time_t time)
{
    char       buffer[80];
    struct tm *timeinfo;

    timeinfo = localtime(&time);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

static std::string sizeOfFile(off_t size)
{
    std::stringstream ss;
    std::string       sizeStr;

    if (size < KB)
    {
        ss << size;
        ss >> sizeStr;
        return sizeStr + "B";
    }
    else if (size < MB)
    {
        ss << size / 1024;
        ss >> sizeStr;
        return sizeStr + "KB";
    }
    else if (size < GB)
    {
        ss << size / (1024 * 1024);
        ss >> sizeStr;
        return sizeStr + "MB";
    }
    else 
    {
        ss << size / (1024 * 1024 * 1024);
        ss >> sizeStr;
        return sizeStr + "GB";
    }
    ss << size;
    ss >> sizeStr;
    return sizeStr;
}

static std::string generateHtmlLine(struct dirent *entry, std::string uri, std::string filePath)
{
    std::stringstream line;
    struct stat       buffer;

    std::string path = filePath + "/" + entry->d_name;
    if (uri[uri.length() - 1] != '/')
        uri.append("/");
    std::cout << "path: " << path << std::endl;
    if (stat(path.c_str(), &buffer) != 0)
        return "";
    if (entry->d_type == DT_DIR && std::strcmp(entry->d_name, "..") != 0)
        line << "<tr><td><a href=\"" << uri << entry->d_name << "\">&#128450; "
             << " "
             << "<b>" << entry->d_name << "/</b></a></td><td>" << formatDateTime(buffer.st_mtime)
             << "</td><td> - </td></tr>\n";
    else if (std::strcmp(entry->d_name, "..") == 0)
        line << "<tr><td><a href=\"" << uri << entry->d_name << "\">&#128228 "
             << " " << entry->d_name << "/</a></td><td> </td><td>  </td></tr>\n";
    else
        line << "<tr><td><a href=\"" << uri << entry->d_name << "\">&#128196;"
             << " " << entry->d_name << "</a></td><td>" << formatDateTime(buffer.st_mtime) << "</td><td>"
             << sizeOfFile(buffer.st_size) << "</td></tr>\n";
    return line.str();
}

static std::string pageStyle(void)
{
    std::stringstream style;

    style << "  <style>\n";
    style << "    * {\n";
    style << "      background: #333;\n";
    style << "      transition: all 0.6s;\n";
    style << "    }\n";
    style << "    html {\n";
    style << "      height: 100%;\n";
    style << "    }\n";
    style << "    body {\n";
    style << "      font-family: 'Lato', sans-serif;\n";
    style << "      margin: 0;\n";
    style << "    }\n";
    style << "    #main {\n";
    style << "      width: 100%;\n";
    style << "    }\n";
    style << "    table {\n";
    style << "      width: 80%;\n";
    style << "      margin: 0 auto;\n";
    style << "      border-collapse: collapse;\n";
    style << "    }\n";
    style << "    th, td {\n";
    style << "      border: 1px solid #333;\n";
    style << "      padding: 6px;\n";
    style << "      text-align: left;\n";
    style << "      color: #fff;\n";
    style << "    }\n";
    style << "    a {\n";
    style << "      color: #fff;\n";
    style << "      text-decoration: none;\n";
    style << "    }\n";
    style << "    h1 {\n";
    style << "      color: #fff;\n";
    style << "      margin-left: 10%;\n";
    style << "      margin-top: 2%;\n";
    style << "      margin-bottom: 2%;\n";
    style << "    }\n";
    style << "  </style>\n";
    return style.str();
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
        chunk_data = "<html>\n<head>\n<title>Index of " + filePath + "</title>\n" + pageStyle() +
                     "</head>\n<body>\n<h1>Index of " + filePath + "</h1>\n<hr>\n<pre>\r\n";

    chunk_data.append("<table>\n");
    while (entry && chunk_data.length() != CLIENT_RESPONSE_BUFFER_SIZE)
    {
        if (entry && !strcmp(entry->d_name, "."))
        {
            entry = readdir(directory);
            continue;
        }
        chunk_data.append(generateHtmlLine(entry, clientIt->fields[URI], filePath));
        entry = readdir(directory);
    }
    chunk_data.append("</table>\n");
    if (!entry)
        chunk_data.append("</pre>\n<hr>\n</body>\n</html>");
    ss << std::hex << chunk_data.length();
    ss >> chunk_size;
    chunk_data += CRLF;
    chunk_size += CRLF;
    if (send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0) < 1 ||
        send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0) < 1)
        throw ResponseSendingException("ResponseSendingException");
    if (!entry)
    {
        chunk_data = CRLF;
        chunk_size = "0" + chunk_data;
        if (send(clientIt->connect_socket, &chunk_size[0], chunk_size.length(), 0) < 1 ||
            send(clientIt->connect_socket, &chunk_data[0], chunk_data.length(), 0) < 1)
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
