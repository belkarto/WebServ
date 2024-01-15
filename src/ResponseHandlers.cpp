#include "../include/Multiplexer.hpp"

void Response::parseFilePath(CLIENTIT &clientIt)
{
    if (access(filePath.c_str(), F_OK)) // file not found
    {
        if (status == STATUS_404)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_404;
        this->setErrorResponse(clientIt);
    }
    else if (access(filePath.c_str(), R_OK)) // permission denied
    {
        if (status == STATUS_403)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_403;
        this->setErrorResponse(clientIt);
    }
    else
    {
        if (is_directory(filePath.c_str()))
            handleDirectory(clientIt);
        else
        {
            if (cgi)
                return (handleCgi(clientIt));
            handleFile(clientIt);
        }
    }
}

void Response::handleDirectory(CLIENTIT &clientIt)
{
    if (!handleIndexPages(clientIt))
    {
        if (!handleAutoIndex(clientIt))
        {
            if (status == STATUS_403) // failure in both index pages and autoindex
                return (handleDefaultErrorPage(clientIt));
            this->resetState();
            status = STATUS_403;
            this->setErrorResponse(clientIt);
        }
    }
}

bool Response::handleIndexPages(CLIENTIT &clientIt)
{
    std::vector<std::string>::iterator it;
    std::string                        fileTmp;

    fileTmp = filePath;
    if (!index || index->empty())
        return false;
    it = index->begin();
    for (; it != index->end(); it++)
    {
        filePath += *it;
        if (!access(filePath.c_str(), F_OK))
        {
            if (!access(filePath.c_str(), R_OK))
                handleFile(clientIt);
            else
            {
                if (status == STATUS_403)
                    handleDefaultErrorPage(clientIt);
                else
                {
                    status = STATUS_403;
                    this->setErrorResponse(clientIt);
                }
            }
            return true;
        }
        filePath = fileTmp;
    }
    return false;
}

void Response::handleFile(CLIENTIT &clientIt)
{
    if (status.empty())
		status = STATUS_200;
    if (cgi)
        fileContent = new std::ifstream(CgiFilePath.c_str());
    else
        fileContent = new std::ifstream(filePath.c_str());
    if (!fileContent || !fileContent->is_open())
    {
        status = STATUS_500;
        return (handleDefaultErrorPage(clientIt));
    }
    response_size = getFileSize(fileContent);
    contentLength = toString(response_size);
    contentType = clientIt->getMimeType(filePath);
    sendHeaders(clientIt);
}

void Response::handleExternalRedirection(CLIENTIT &clientIt)
{

    status = STATUS_302;
    if (location.empty()) // if location snt empty => location = errorPageURI
        location = clientIt->locatIt->redirect;
    special_response = "<html>" CRLF "<head><title>";
    special_response.append(STATUS_302)
        .append("</title></head>" CRLF "<body>" CRLF "<center><h1>")
        .append(STATUS_302)
        .append("</h1></center>" CRLF "</body></html>" CRLF);
    contentLength = toString(special_response.length());
    sendHeaders(clientIt);
    if (send(clientIt->connect_socket, &special_response[0], special_response.length(), 0) < 1)
        throw ResponseSendingException("ResponseSendingException");
    clientIt->response_all_sent = true;
}

bool Response::handleAutoIndex(CLIENTIT &clientIt)
{

    if (!autoindex)
        return false;
    status = STATUS_200;
    directory = opendir(filePath.c_str());
    if (!directory)
    {
        if (status == STATUS_500)
            handleDefaultErrorPage(clientIt);
        else
        {
            this->resetState();
            status = STATUS_500;
            setErrorResponse(clientIt);
        }
        return true;
    }
    transferEncoding = "chunked";
    sendHeaders(clientIt);
    return true;
}

void Response::handleDefaultErrorPage(CLIENTIT &clientIt)
{
    std::stringstream ss;
    int               code;

    ss << status;
    ss >> code;
    special_response = getErrorPage(code);
    contentLength = toString(special_response.length());
    sendHeaders(clientIt);
    if (send(clientIt->connect_socket, &special_response[0], special_response.length(), 0) < 1)
         throw ResponseSendingException("ResponseSendingException");
    clientIt->response_all_sent = true;
}

void Response::parsePostFilePath(CLIENTIT &clientIt)
{

    if (access(filePath.c_str(), F_OK)) // file not found
    {
        if (status == STATUS_404)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_404;
        this->setErrorResponse(clientIt);
    }
    else if (access(filePath.c_str(), W_OK) || *(--filePath.end()) == '/') // permission denied
    {
        if (status == STATUS_403)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_403;
        this->setErrorResponse(clientIt);
    }
    else if (!is_directory(filePath.c_str()))
    {
        if (status == STATUS_405)
            return (handleDefaultErrorPage(clientIt));
        this->resetState();
        status = STATUS_405;
        this->setErrorResponse(clientIt);
    }
}

void Response::handleCgi(CLIENTIT &clientIt)
{
    std::string content_type;
    char        *cmds[3];

    cmds[0] = const_cast<char *>(cgiExecutable.c_str());
    cmds[1] = const_cast<char *>(filePath.c_str());
    cmds[2] = NULL;
    content_type = "text/plain";
    CgiFilePath = "/tmp";
    CgiFilePath.append(clientIt->generateFileName(content_type)).append(".cgi");
    if (access(cmds[0], F_OK))
    {
        resetState();
        status = STATUS_404;
        setErrorResponse(clientIt);
    }
    else if (access(cmds[0], X_OK))
    {
        resetState();
        status = STATUS_403;
        setErrorResponse(clientIt);
    }
    else
    {
        if ((pid = fork()) < 0)
        {
            resetState();
            status = STATUS_500;
            return (setErrorResponse(clientIt));
        }
        if (!pid)
        {
            Multiplexer::env = setCgiEnv(clientIt);
            if (clientIt->fields["method"] == "POST")
            {
                std::freopen(outFileCgiPath.c_str(), "r", stdin);
            }
            std::freopen(CgiFilePath.c_str(), "w+", stdout);
            std::freopen(CgiFilePath.c_str(), "w+", stderr);
            execve(cmds[0], cmds, Multiplexer::env);
        }
        else
        {
            counter = time(NULL);
            checkCgiTimeout(clientIt);
        }
    }
}

void Response::checkCgiTimeout(CLIENTIT &clientIt)
{
    int wstatus;
    int wpid;

    wpid = waitpid(pid, &wstatus, WNOHANG);
    if (wpid < 0)
    {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        unlink(CgiFilePath.c_str());
        resetState();
        status = STATUS_500;
        setErrorResponse(clientIt);
    }
    else if (wpid > 0)
    {
        if (WIFEXITED(wstatus))
        {
            if (WEXITSTATUS(wstatus) != 0)
            {
                unlink(CgiFilePath.c_str());
                resetState();
                status = STATUS_500;
                return (setErrorResponse(clientIt));
            }
            handleFile(clientIt);
        }
    }
    else if ((time(NULL) - counter) >= CGI_TIMEOUT)
    {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        unlink(CgiFilePath.c_str());
        resetState();
        status = STATUS_408;
        setErrorResponse(clientIt);
    }
}

void Response::handleDelete(CLIENTIT &clientIt)
{
    int ecode;

    if (remove_all(filePath.c_str(), ecode) < 0)
    {
        if (ecode == ENOENT)
            status = STATUS_404;
        else if (ecode == EACCES || ecode == ENOTEMPTY || ecode == EISDIR)
            status = STATUS_403;
        else if (ecode == EBUSY)
            status = STATUS_409;
        else
            status = STATUS_500;
        setErrorResponse(clientIt);
    }
    else
    {
        status = STATUS_204;
        contentLength = contentType = "";
        sendHeaders(clientIt);
        if (send(clientIt->connect_socket, &special_response[0], special_response.length(), 0) < 1)
            throw ResponseSendingException("ResponseSendingException");
        clientIt->response_all_sent = true;
    }
}

int setEcode(int &ecode)
{
    ecode = errno;
    return -1;
}

int remove_all(const char *path, int &ecode)
{
    struct stat    statbuf;
    DIR           *dir;
    struct dirent *entry;
    std::string    new_path;

    if (access(path, W_OK))
        return (setEcode(ecode));
    if (stat(path, &statbuf) < 0)
        return (setEcode(ecode));
    if (S_ISDIR(statbuf.st_mode))
    {
        if ((dir = opendir(path)) == NULL)
            return (setEcode(ecode));
        while ((entry = readdir(dir)) != NULL)
        {
            if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, "."))
                continue;
            new_path = path;
            if (new_path[new_path.length() - 1] != '/')
                new_path.append("/");
            new_path.append(entry->d_name);
            remove_all(new_path.c_str(), ecode);
        }
        closedir(dir);
        if (remove(path) < 0)
            return (setEcode(ecode));
    }
    else if (remove(path) < 0)
        return (setEcode(ecode));
    return 0;
}
