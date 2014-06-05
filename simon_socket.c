#include"simon_socket.h"

extern int errno;

void exit_error(char *fmt, ...)
{
	char *p_tmp = fmt, *s_tmp;

	va_list ap;
	va_start(ap, fmt);

	while (*p_tmp)   //  * can't drop
	{
		if (*p_tmp != '%')
		{
			putchar(*p_tmp++);
			continue;
		}
		switch (*++p_tmp)
		{
			case 'd':{
						printf("%d", va_arg(ap, int));
						break;
					 }
			case 's':{
						 for (s_tmp = va_arg(ap, char*); *s_tmp; s_tmp++)
							 putchar(*s_tmp);
						 break;
					 }
			default:
					 putchar(*p_tmp);
		}
		p_tmp++;
	}
	va_end(ap);

	exit(1);
}

int init_passivesock(char *transport_server, int port)
{
	int sockfd, type, addr_len;
	struct sockaddr_in local_addr;

	if (!strcmp("udp", transport_server))
		type = SOCK_DGRAM;
	else if (!strcmp("tcp", transport_server))
		type = SOCK_STREAM;

	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET; 
	local_addr.sin_port = htons(port);
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	addr_len = sizeof(struct sockaddr_in);

	if ((sockfd = socket(AF_INET, type, 0)) < 0)
		exit_error("Fail to init %s socket: %s", transport_server, strerror(errno));

	int opt = SO_REUSEADDR;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(struct sockaddr)) < 0)
		exit_error("Fail to bind %s socket on %d: %s", transport_server, port, strerror(errno));

	if (type == SOCK_STREAM)
	{
		if (listen(sockfd, MAX_BACKLOG) < 0)
			exit_error("Fail to listening on port %d: %s", port, strerror(errno));
	}

	return sockfd;
}

int init_udp_psock(int port)
{
	return init_passivesock("udp", port);
}

int init_tcp_psock(int port)
{
	return init_passivesock("tcp", port);
}

/* return the short count */
int simon_send(int fd, char* buf, unsigned int n)
{
	int left = n;	
	char *bufptr = buf;
	int send_bytes;
	
	while (left >= 0)
	{
		if ((send_bytes = send(fd, bufptr, MAX_BUF_SIZE, 0)) < 0)  
		{
			if (errno == EINTR)  //iterrupted by signal , send again
				send_bytes = 0;
			else
				break;
		}
		else if (!send_bytes)  // EOF or socket shutdown by peer
			break;

		left -= send_bytes;
		bufptr += send_bytes;
	}

	return n - left;
}

int simon_recv(int fd, char* buf, unsigned int n)
{
	int left = n;	
	char *bufptr = buf;
	int recv_bytes;
	
	while (left >= 0)
	{
		if ((recv_bytes = recv(fd, bufptr, MAX_BUF_SIZE, 0)) < 0)
		{
			if (errno == EINTR)  
				recv_bytes = 0;
			else
				return -1;
		}
		else if (!recv_bytes) 
			break;

		left -= recv_bytes;
		bufptr += recv_bytes;
	}

	return n - left;
}

int init_initiativesock(char *transport_server, char *addr, int port)
{
	int sockfd, type;
	struct sockaddr_in remote_addr;

	if (!strcmp("udp", transport_server))
		type = SOCK_DGRAM;
	else if (!strcmp("tcp", transport_server))
		type = SOCK_STREAM;

	if ((sockfd = socket(AF_INET, type, 0)) < 0)
		exit_error("Fail to init %s socket: %s", transport_server, strerror(errno));

	printf("Success to init %s socket!", transport_server);

	if (type == SOCK_STREAM)
	{
		memset(&remote_addr, 0, sizeof(remote_addr));
		remote_addr.sin_family = AF_INET; 
		remote_addr.sin_port = htons(port);
		remote_addr.sin_addr.s_addr = inet_addr(addr);

		if (connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr)) == -1)
			exit_error("Fail to connect with the remote host: %s", strerror(errno));

		printf("Success to connect with %s\n", inet_ntoa(remote_addr.sin_addr));
	}

	return sockfd;
}

int init_udp_isock()
{
	return init_initiativesock("udp", NULL, 0);
}

int init_tcp_isock(char *addr, int port)
{
	return init_initiativesock("tcp", addr, port);
}

