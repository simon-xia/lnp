/* 
 * Epoll server 
 *				use ET(edge-triggered) work option. Only support no-block socket
 * 
 *								Thu Apr 10 10:43:03 CST 2014
 *								by Simon Xia
 */
#include"simon_socket.h"
#include<fcntl.h>
#include<sys/epoll.h>

#define SERV_PORT 12345
#define MAX_EPOLLFD  100
#define EVENT_SIZE 90

int set_fd_nonblocking(int fd)
{
	int flag;

	flag = fcntl(fd, F_GETFL, 0);
	if (flag == -1)
	{
		perror("fcntl error: ");
		return -1;
	}

	flag |= O_NONBLOCK;

	if (fcntl(fd, F_SETFD, flag) == -1)
	{
		perror("fcntl error: ");
		return -1;
	}
	return 0;
}

int main()
{
	int i, listenfd, contfd, epfd, readyfd, curfd = 1, recv_bytes;
	struct sockaddr_in cli_addr;
	struct epoll_event ev_tmp, events[EVENT_SIZE];

	size_t addr_len = sizeof(struct sockaddr);

	epfd = epoll_create(MAX_EPOLLFD);

	listenfd = init_tcp_psock(SERV_PORT);

	ev_tmp.data.fd = listenfd;
	ev_tmp.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev_tmp) == -1)
	{
		perror("Add event failed: ");	
		return 1;
	}

	printf("Epoll server startup at port %5d\n", SERV_PORT);

	while(1)
	{
		readyfd = epoll_wait(epfd, events, EVENT_SIZE, -1);

		for (i = 0; i < readyfd; i++)
		{
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP))
			{
				perror("Epoll error: ");
				close(events[i].data.fd);
				continue;
			}
			else if (events[i].data.fd == listenfd)	
			{
				if ((contfd = accept(listenfd, (struct sockaddr *)&cli_addr, &addr_len)) == -1)
				{
					perror("Accept request failed: ");
					return 1;
				}
				else
					printf("Get a connection from %s:%5d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

				if (curfd > EVENT_SIZE)
				{
					printf("Too many connections, more than %d\n", EVENT_SIZE);
					continue;
				}

				set_fd_nonblocking(contfd);
				
				ev_tmp.data.fd = contfd;
				ev_tmp.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, contfd, &ev_tmp);

				curfd++;
				continue;
			}
			else if (events[i].events & EPOLLIN)
			{
				if ((recv_bytes = recv(events[i].data.fd, recv_buf, MAX_BUF_SIZE, 0)) <= 0)
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL); 
					getpeername(events[i].data.fd, (struct sockaddr *)&cli_addr, &addr_len);
					printf("%s:%5d quit the connection\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
					close(events[i].data.fd);
					curfd--;
				}
				else
				{
                    ev_tmp.data.fd = events[i].data.fd;
                    ev_tmp.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &ev_tmp);
				}
			}
			else if (events[i].events & EPOLLOUT)
			{
				send(events[i].data.fd, recv_buf, recv_bytes, 0);

				ev_tmp.data.fd = events[i].data.fd;
				ev_tmp.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &ev_tmp);
			}
		}
	}
	close(listenfd);
	return 0;
}

