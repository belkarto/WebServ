#ifndef DEFINE_HPP
# define DEFINE_HPP

# define DEFAULT_CONF_PATH				"config/default.conf"
# define DEFAULT_HOST 					"localhost"
# define DEFAULT_PORT					"1337"
# define DEFAULT_ROOT					"./html"
# define DEFAULT_INDEX					"index.html"
# define DEFAULT_AUTOINDEX				false
# define DEFAULT_CLIENT_MAX_BODY_SIZE	1048576	// 1M

/*
    10GB --> 107374182400B
    5GB --> 5368709120
    1GB --> 1073741824
    9223372036GB --> MAX LONG LONG 9223372036854775807
*/
# define MAX_REQUEST_SIZE 107374182400

# define NUM_SERV_DIREC 8
# define NUM_LOCAT_DIREC 7

#endif