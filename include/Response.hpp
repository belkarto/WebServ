#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"
#include <ios>

class Response
{
  public:
    std::string              status; // reponse headers fields
    std::string              connection;
    std::string              contentType;
    std::string              contentLength;
    std::string              transferEncoding;
    std::string              location;
    std::vector<std::string> cookies;

    std::string filePath;
    std::string root;
    STRINGVECT *index;
    bool        autoindex;

    std::string cgiExecutable; // cgi
    std::string CgiFilePath;
    bool        cgi;
    time_t      counter;
    pid_t       pid;

    std::string     special_response; // response
    std::ifstream  *fileContent;
    std::ofstream  *outFile;
    DIR            *directory;
    std::streamsize response_size;
    std::streamsize request_body_size;
    std::streamsize request_read;
    std::streamsize readbytes;

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
    void        handleDelete(CLIENTIT &clientIt);
    void        handleCgi(CLIENTIT &clientIt);
    void        checkCgiTimeout(CLIENTIT &clientIt);
    void        parseCgi(void);
    std::string getErrorPage(int errorCode);

    // post
    bool        postCgi;
    std::string outFileCgiPath;
    std::string fileLocation;
    bool        filePathParsed;
    char       *cgiEnv[10];

    void   postParseFilePath(CLIENTIT &);
    void   parseUploadPath();
    int    parseResourcePath();
    void   ProcessUploadLocation(CLIENTIT &);
    void   processResourceRequest(CLIENTIT &);
    void   handleResourceFile(CLIENTIT &);
    void   handleResourceDire(CLIENTIT &);
    void   parsePostFilePath(CLIENTIT &);
    // void   getUnprocessedHeaders(CLIENTIT &);
    char **setCgiEnv(CLIENTIT &);
    void   recvRequestBody(CLIENTIT &);
};
int remove_all(const char *path, int &ecode);
#endif
