#include "../include/Multiplexer.hpp"
#include "webserv.hpp"

static char **joinEnvp(char **envp, char **newEnvp)
{
    int    i;
    int    j;
    char **tmp;

    i = -1;
    while (envp[++i]) ;
    j = -1;
    while (newEnvp[++j]) ;
    tmp = new char *[i + j + 1];
    i = -1;
    while (envp[++i])
        tmp[i] = envp[i];
    j = -1;
    while (newEnvp[++j])
        tmp[i + j] = newEnvp[j];
    tmp[i + j] = NULL;
    // delete[] envp;
    delete[] newEnvp;
    return tmp;
}

// set post cgi environment variables
char **Response::setPostCgiEnv(char **envp, CLIENTIT &clientIt)
{
    // CONTENT_LENGTH
    // CONTENT_TYPE
    // HTTP_HOST
    // query_string

    std::string content = "CONTENT_LENGTH=" + clientIt->fields["Content-Length"];
    std::string type = "CONTENT_TYPE=" + clientIt->fields["Content-Type"];
    char      **newEnvp = new char *[5];
    newEnvp[0] = strdup(content.c_str());
    newEnvp[1] = strdup(type.c_str());
    newEnvp[2] = strdup(("HTTP_HOST=" + clientIt->serverIt->bind_addr_str).c_str());
    newEnvp[3] = NULL;
    newEnvp[4] = NULL;
    return joinEnvp(envp, newEnvp);
}
