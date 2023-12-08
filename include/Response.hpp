#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"

class Response
{
  public:
    std::string status; // reponse headers fields
    std::string connection;
    std::string contentType;
    std::string contentLength;
    std::string transferEncoding;
    std::string location;

    std::string filePath;
    std::string fileLocation;
    std::string root;
    std::string cgiExecutable;
    STRINGVECT *index;
    bool        autoindex;

    bool   cgi;
    time_t counter;
    int    fds[2];
    pid_t  pid;

    int             code;
    std::string     special_response;
    int             fd;
    std::ifstream  *fileContent;
    std::ofstream  *outFile;
    DIR            *directory;
    std::streamsize response_size;
    std::streamsize readbytes;
    bool            filePathParsed;

    Response(void);
    ~Response();
    void resetState();

    /*		error handling				*/
    void setErrorResponse(CLIENTIT &clientIt);
    /*		Get method					*/
    void setGetResponse(CLIENTIT &clientIt);
    /*		POST method					*/
    void setPostResponse(CLIENTIT &clientIt);
    /*		DELETE method					*/
    void setDeleteResponse(CLIENTIT &clientIt);

    /*  common methods  */
    void        handleDefaultErrorPage(CLIENTIT &clientIt);
    void        parseFilePath(CLIENTIT &clientIt);
    void        handleExternalRedirection(CLIENTIT &clientIt);
    void        handleDirectory(CLIENTIT &clientIt);
    void        handleFile(CLIENTIT &clientIt);
    bool        handleIndexPages(CLIENTIT &clientIt);
    bool        handleAutoIndex(CLIENTIT &clientIt);
    void        sendHeaders(CLIENTIT &clientIt);
    void        sendResponseBuffer(CLIENTIT &clientIt);
    void        sendAutoIndexBuffer(CLIENTIT &clientIt);
    void        sendPipeBuffer(CLIENTIT &clientIt);
    void        handleDelete(CLIENTIT &clientIt);
    void        handleCgi(CLIENTIT &clientIt);
    void        checkCgiTimeout(CLIENTIT &clientIt);
    std::string getErrorPage(int errorCode);
    // post
    void postParseFilePath(CLIENTIT &);
    void parseUploadPath(CLIENTIT &);
    int  parseResourcePath(CLIENTIT &);
    void ProcessUploadLocation(CLIENTIT &);
    void processResourceRequest(CLIENTIT &);
};
int remove_all(const char *path, int &ecode);
#endif
