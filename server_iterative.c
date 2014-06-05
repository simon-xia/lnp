/* Copyright (c) 2014.3 by Simon Xia
 *
 *  server with tcp connection use iterative model
 */
#include"simon_socket.h"

#define SERVER_PORT 12345

char recv_buf[MAX_BUF_SIZE];

void convert_upper(char * buf, int len)
{
	int i = 0;
	while (i != len)
	{
		if (buf[i] >= 97 && buf[i] <= 122)
			buf[i] -= 32;
		i++;
	}
}

void process_client(int connectfd, struct sockaddr_in *client)
{
	int recv_bytes, send_bytes;
	while (1)
	{
		memset(recv_buf, 0, MAX_BUF_SIZE);
		printf("\n**********************\n");
		if ((recv_bytes = recv(connectfd, recv_buf, MAX_BUF_SIZE, 0)) > 0)
		{
			printf("%d Bytes data received from%s:\n%s\n", recv_bytes, inet_ntoa(client -> sin_addr), recv_buf);
		}
		else
			printf("fail to receive data!\n");

		if (!strcmp(recv_buf, "quit"))
			break;

		convert_upper(recv_buf, recv_bytes);

		if ((send_bytes = simon_send(connectfd, recv_buf, strlen(recv_buf)+1, 0)) > 0)
			{
				printf("%d Bytes data send to %s:\n%s\n", recv_bytes, inet_ntoa(client -> sin_addr), recv_buf);
			}
			else
				printf("fail to send data!\n");
			
		printf("**********************\n");
	}
	
	printf("%s quit the connection!\n", inet_ntoa(client -> sin_addr));
	close(connectfd);
}

int main()
{
	int sockfd, acfd;
	struct sockaddr_in client_addr;

	int sin_len = sizeof(struct sockaddr);

	sockfd = init_tcp_psock(SERVER_PORT);

	printf("server started successfully! Listening on the port: %d\n", SERVER_PORT);

	while (1)
	{
		if ((acfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_len)) == -1)
		{
			perror("Accept request failed: ");
			return 1;
		}
		else
			printf("Get a connection from %s !\n", inet_ntoa(client_addr.sin_addr));
		process_client(acfd, &client_addr);
	}
	close(sockfd);
	return 0;

}
