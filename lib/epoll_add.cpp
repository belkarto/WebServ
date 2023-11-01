#include "webserv.hpp"

void	epoll_add(int epfd, int fd)
{
	struct epoll_event						event;

	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) < 0)
	{
		perror("epoll_ctl()");
		exit(EXIT_FAILURE);
	}
}

void	epoll_delete(int epfd, int fd)
{
	struct epoll_event						event;

	event.data.fd = fd;
	event.events = EPOLLIN;
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event) < 0)
	{
		perror("epoll_ctl()");
		exit(EXIT_FAILURE);
	}
}