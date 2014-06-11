 /* 
 *   server with multi-thread 
 *				Fri Mar 21 22:12:47 CST 2014
 *              by  Simon Xia
 */
#include"simon_socket.h"

#define SERV_PORT 12345

typedef struct arg_tag
{
	int connectfd;
	struct sockaddr_in  addr_info; //不用指针，入口地址会被改写
}ARG;


void *start_routine(void *arg)
{ 
	pthread_detach(pthread_self());
/*
	ARG tmp;
	tmp.connectfd = ((ARG*)arg) -> connectfd;
	tmp.addr_info = ((ARG*)arg) -> addr_info;
	process_client(tmp.connectfd, &tmp.addr_info);
	close(tmp.connectfd);

*/
	process_client(((ARG*)arg) -> connectfd, &((ARG*)arg) -> addr_info);//指针传地址，地址信息显示有误
	close(((ARG *)arg) -> connectfd);
	
	//free(arg);
	pthread_exit(NULL);
}

int main()
{
	int sockfd, acfd;
	size_t sin_len = sizeof(struct sockaddr);
	ARG *arg;

	struct sockaddr_in client_addr;

	pthread_t thread;

	sockfd = init_tcp_psock(SERV_PORT);
	
	while (1)
	{
		if ((acfd = accept_request(sockfd, (struct sockaddr *)&client_addr, &sin_len)) <= 0)
			continue;

		arg = malloc(sizeof(ARG));
		arg -> connectfd = acfd;
		arg -> addr_info = client_addr;

		if (pthread_create(&thread, NULL, start_routine, (void*)arg))
		{
			printf("Create thread error!\n");
			continue;
		}
		free(arg);
	}
	close(sockfd);
	return 0;

}
