#include "webserv.hpp"

void	sig_handler(int signum)
{
	if (signum == SIGINT)
		exit(143);
}