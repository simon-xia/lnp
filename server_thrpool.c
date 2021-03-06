 /* 
 *   server with thread pool to realize concurrence 
 *				Mon Jun 9 21:50:36 CST 2014
 *              by  Simon Xia
 */

#include"simon_socket.h"

#define SERV_PORT 12345
#define THREAD_CNT 10

extern void pool_init(int );
extern int add_task(void* (*) (void*), void*);
extern void pool_destory();

typedef struct client_info{
	int fd;
	struct sockaddr_in addr;
	struct client_info *next;
}client_info;

void *process(void *arg)
{
	process_client(((client_info*)arg)->fd, &((client_info*)arg)->addr);
	return NULL;
}

void sig_int(int signo)
{
	pool_destory();
	exit(0);
}

int main()
{
	int sockfd, acfd;
	size_t sin_len;
	struct sockaddr_in client_addr;
	client_info *info_tmp, *info_head = NULL, *info_tail = NULL;

	signal(SIGINT, sig_int);

	sin_len = sizeof(struct sockaddr);
	sockfd = init_tcp_psock(SERV_PORT);
	pool_init(THREAD_CNT);


	for ( ; ; )
	{
		if ((acfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_len)) == -1)
		{
			perror("Accept request failed: ");
			return 1;
		}
		else
			printf("Get a connection from %s:%d !\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		info_tmp = (client_info *)malloc(sizeof(client_info));
		memset(info_tmp, 0, sizeof(client_info));
		info_tmp->fd = acfd;
		info_tmp->addr = client_addr;
		info_tmp->next = NULL;
		
		if (info_head) {
			info_tail = info_tail -> next = info_tmp;
		}
		else{
			info_head = info_tail = info_tmp;
		}
		add_task(process, (void*)info_tmp);
	}

	for (info_tmp = info_head; info_tmp; free(info_tmp))
		info_head = info_head -> next;

	return 0;

