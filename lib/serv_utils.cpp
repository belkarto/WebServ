#include "WebServ.hpp"

void	sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << "Process Terminated" << std::endl;
		exit(143);
	}
}