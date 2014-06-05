#include"simon_socket.h"

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

	if (strcmp("udp", transport_server))
		type = SOCK_DGRAM;
	else if (strcmp("tcp", transport_server))
		type = SOCK_STREAM;

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
