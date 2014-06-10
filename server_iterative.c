/* Copyright (c) 2014.3 by Simon Xia
 *
 *  server with tcp connection use iterative model
 */
#include"simon_socket.h"

#define SERVER_PORT 12345

int main()
{
	int sockfd, acfd;
	struct sockaddr_in client_addr;

	size_t sin_len = sizeof(struct sockaddr);

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
