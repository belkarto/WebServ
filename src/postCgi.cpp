#include "../include/Multiplexer.hpp"
#include "webserv.hpp"

// static char **joinEnvp(char **envp, char **newEnvp)
// {
//     int    i;
//     int    j;
//     char **tmp;
//
//     i = -1;
//     while (envp[++i])
//         ;
//     j = -1;
//     while (newEnvp[++j])
//         ;
//     tmp = new char *[i + j + 1];
//     i = -1;
//     while (envp[++i])
//         tmp[i] = envp[i];
//     j = -1;
//     while (newEnvp[++j])
//         tmp[i + j] = newEnvp[j];
//     tmp[i + j] = NULL;
//     // delete[] envp;
//     delete[] newEnvp;
//     return tmp;
// }
//
// set post cgi environment variables
char **Response::setPostCgiEnv(char **envp, CLIENTIT &clientIt)
{
    char **newEnvp = new char *[9];
    newEnvp[0] = strdup(("SERVER_PROTOCOL=" + clientIt->fields["protocol_version"]).c_str());
    newEnvp[1] = strdup(("CONTENT_LENGTH=" + clientIt->fields["Content-Length"]).c_str());
    newEnvp[2] = strdup(("CONTENT_TYPE=" + clientIt->fields["Content-Type"]).c_str());
    // newEnvp[2] = strdup(("CONTENT_TYPE=application/x-www-form-urlencoded"));
    newEnvp[3] = strdup(("HTTP_HOST=" + clientIt->serverIt->bind_addr_str).c_str());
    newEnvp[4] = strdup(("HTTP_COOKIE=" + clientIt->fields["Cookie"]).c_str());
    newEnvp[5] = strdup(("REQUEST_METHOD=" + clientIt->fields["method"]).c_str());
    newEnvp[6] = strdup(("REDIRECT_STATUS=" + status).c_str());
    newEnvp[7] = strdup(("SCRIPT_FILENAME=" + clientIt->response.filePath).c_str());
    newEnvp[8] = NULL;
    (void)envp;
    return newEnvp;
}
