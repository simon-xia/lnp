 /* 
  *   a tcp client use iterative model
  *				Fri Mar 14 21:34:10 CST 2014
  *             by  Simon Xia
  */
#include"simon_socket.h"

#define SERV_PORT 12345

int writetobuf()
{
	int i = 0, c_tmp;
	while ( i < MAX_BUF_SIZE && (c_tmp = getchar()) != EOF)
	{
		send_buf[i++] = c_tmp;
	}
	return i;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <IP Addrees>\n", argv[0]);
		return 1;
	}
	//freopen("input","r",stdin);
	int sockfd, send_bytes, recv_bytes;

	sockfd = init_tcp_isock(argv[1], SERV_PORT);

	while (1)
	{
		memset(send_buf, 0, MAX_BUF_SIZE);
		printf("Input the data you wanna send(Ctrl + D to mark the end, input \"quit\" to quit):\n");

		int s_size = writetobuf();

		if ((send_bytes = send(sockfd, send_buf, s_size, 0)) > 0)
		{
			printf("\nSuccess to send %d Bytes data!\n", send_bytes);
		}
		else
			printf("send data failed!\n");

		if (!strcmp(send_buf, "quit"))
			break;

		memset(recv_buf, 0, MAX_BUF_SIZE);
		if ((recv_bytes = recv(sockfd, recv_buf, MAX_BUF_SIZE, 0)) > 0)
			printf("Success to recieve %d Bytes data:\n%s\n", recv_bytes, recv_buf);
		else
			printf("Fail to receive data!\n");
	}
	
	close(sockfd);
	printf("Success to close the connection!\n");

	return 0;
}
