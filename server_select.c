/* 
 *  server with tcp connection use select to realize I/O multiplexing
 *				Mon Jun 2 21:44:36 CST 2014
 *              by  Simon Xia
 */
#include "simon_socket.h"

#define SERV_PORT 12345
#define FDSET_SIZE 32

typedef struct Clientinfo{
	int fd;
	struct sockaddr_in addr;
}Clientinfo;

typedef struct Clientpool{
	int count;
	Clientinfo cinfo_set[FDSET_SIZE];
}Clientpool;

void init_clientpool(Clientpool *pool) 
{
	int i;
	pool->count = 0;
	memset(pool->cinfo_set, 0, sizeof(pool->cinfo_set));
	for (i = 0; i < FDSET_SIZE; i++)
		(pool->cinfo_set[i]).fd = -1;
}

void add_clientinfo(Clientpool *pool, int newfd, struct sockaddr_in client) // change
{
	int i;
	for (i = 0; i < FDSET_SIZE; i++)
	{
		if (pool->cinfo_set[i].fd < 0)
		{
			pool->cinfo_set[pool->count].fd = newfd;
			memcpy((char*)&(pool->cinfo_set[pool->count].addr), (char*)&client, sizeof(struct sockaddr_in));
			pool->count++;
			break;
		}
	}
}

int process_cli(Clientinfo cli)
{
	int recv_bytes, send_bytes;
	
	if ((recv_bytes = recv(cli.fd, recv_buf, MAX_BUF_SIZE, 0)) < 0)	
	{
		perror("Fail to recieve data");
	}
	else if (!recv_bytes)
		return -1;

	printf("Success to recieve %d bytes data from %s:%d\n%s\n", recv_bytes, inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), recv_buf);
	if ((send_bytes = send(cli.fd, recv_buf, recv_bytes, 0)) < 0)
	{
		perror("Fail to send data");
	}
	printf("Success to send %d bytes data to %s:%d\n%s\n", recv_bytes, inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), recv_buf);
	
	return 0;
}

int main()
{
	int sockfd, retval, connfd, i, maxfd;
    size_t addr_len; 
	struct sockaddr_in client_addr;
	fd_set fdset, watchset;
	Clientpool cpool;

	addr_len = sizeof(struct sockaddr);
	init_clientpool(&cpool);

	sockfd = init_tcp_psock(SERV_PORT);
	
	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);
	maxfd = sockfd;

	for (; ;)
	{
		watchset = fdset;   //select 调用返回将修改fdset
		retval = select(maxfd+1, &watchset, NULL, NULL, NULL);  //两个同时连接，会不会排队？

		if (retval < 0)
		{
			perror("Select error");
			continue;
		}
		else
		{
			while (retval--)
			{
				if (FD_ISSET(sockfd, &watchset))
				{
					if ((connfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len)) == -1)
					{
						perror("Fail to accept the connection");
						continue;
					}
					printf("Get a connetion from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					FD_SET(connfd, &fdset);
					add_clientinfo(&cpool, connfd, client_addr);
					if ( connfd > maxfd ) maxfd = connfd;  //mark
				}
				else 
				{
					for (i = 0; i < cpool.count; i++)
					{
						if (cpool.cinfo_set[i].fd < 0) //mark
							continue;
						if (FD_ISSET(cpool.cinfo_set[i].fd, &watchset))
						{
							if (process_cli(cpool.cinfo_set[i]) < 0)
							{
								printf("%s:%d quit the connection\n", inet_ntoa(cpool.cinfo_set[i].addr.sin_addr), ntohs(cpool.cinfo_set[i].addr.sin_port));
								FD_CLR(cpool.cinfo_set[i].fd, &fdset);
								close(cpool.cinfo_set[i].fd);
								cpool.count--;
								cpool.cinfo_set[i].fd = -1;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
