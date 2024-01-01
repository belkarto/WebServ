#include "../include/Multiplexer.hpp"
#include "webserv.hpp"

char **Response::setPostCgiEnv(CLIENTIT &clientIt)
{
    cgiEnv[0] = strdup(("SERVER_PROTOCOL=" + clientIt->fields["protocol_version"]).c_str());
    cgiEnv[1] = strdup(("CONTENT_LENGTH=" + clientIt->fields["Content-Length"]).c_str());
    cgiEnv[2] = strdup(("CONTENT_TYPE=" + clientIt->fields["Content-Type"]).c_str());
    cgiEnv[3] = strdup(("HTTP_HOST=" + clientIt->serverIt->bind_addr_str).c_str());
    cgiEnv[4] = strdup(("HTTP_COOKIE=" + clientIt->fields["Cookie"]).c_str());
    cgiEnv[5] = strdup(("REQUEST_METHOD=" + clientIt->fields["method"]).c_str());
    cgiEnv[6] = strdup(("REDIRECT_STATUS=" + status).c_str());
    cgiEnv[7] = strdup(("SCRIPT_FILENAME=" + clientIt->response.filePath).c_str());
    cgiEnv[8] = NULL;
    return cgiEnv;
}
