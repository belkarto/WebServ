#ifndef __REQUEST_HPP
#define __REQUEST_HPP

#include "webserv.hpp"
class Request
{
  public:
    std::string  root;
    STRINGVECTIT indexIt;
    void         getPostRequest(CLIENTIT &);
};

#endif // !__REQUEST_HPP
