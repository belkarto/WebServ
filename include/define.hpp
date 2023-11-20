#ifndef DEFINE_HPP
# define DEFINE_HPP

#include <vector>
#include <string>
class Server;
class Client;

# define SERVER "webserv"
# define DEF_ERR_PATH "www/error_pages/"
# define PROTOCOL_VERS "HTTP/1.1 "

/*                  server default configuration                */
# define DEFAULT_CONF_PATH				"config/default.conf"
# define DEFAULT_HOST 					"localhost"
# define DEFAULT_PORT					"1337"
# define DEFAULT_ROOT					"./html"
# define DEFAULT_INDEX					"index.html"
# define DEFAULT_AUTOINDEX				false
# define DEFAULT_CLIENT_MAX_BODY_SIZE	1048576	// 1M

# define MAX_REQUEST_SIZE 107374182400  
                                        /*
                                            10GB --> 107374182400B
                                            5GB --> 5368709120
                                            1GB --> 1073741824
                                             9223372036GB --> MAX LONG LONG 9223372036854775807
                                        */
/*                  ConfigParser                                */
# define NUM_SERV_DIREC 8
# define NUM_LOCAT_DIREC 7

/*                  Multiplexer                                  */
# define Running                    true
# define HEADERS_FIELDS_SIZE        5
# define MAX_EVENTS                 1024
# define BUFFER_SIZE                1024
# define CLIENT_HEADER_BUFFER_SIZE  1024
# define CLIENT_HEADER_timeout      30
# define KEEPALIVE_CONNECTIONS      500
# define KEEPALIVE_TIMEOUT          60
# define BACKLOG                    500
# define MIMETYPE_PATH              "./config/mimes.txt"

/*                  typedef                                     */
typedef std::vector<Client>  CLIENTVECT;
typedef std::vector<Server> SERVVECT;
typedef std::vector<Client>::iterator  CLIENTIT;
typedef std::vector<Server>::iterator SERVIT;
typedef std::vector<std::string> STRINGVECT;
typedef std::vector<int> INTVECT;
/*                404 error page in case no default found */ 
#define NOT_FOUND "<!DOCTYPE html>\n<html>\n<head>\n<title>404 - Page Not Found</title>\n</head>\n<body>\n<h1>404 - Page Not Found</h1>\n<p>We're sorry, but the page you requested could not be found.</p>\n</body>\n</html>"
#endif
