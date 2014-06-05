#ifndef _SIMON_SOCKET_H_
#define _SIMON_SOCKET_H_

#include<stdarg.h> // va_list etc
#include<stdio.h>
#include<stdlib.h>

#include<sys/socket.h>   // for socket, bind, listen, setsockopt
#include<sys/types.h>
#include<netinet/in.h>   //sockaddr_in
#include<arpa/inet.h>    //htons etc

#include<errno.h>   // for perror, errno

#define MAX_BACKLOG 10 //default 20

extern int errno;

void exit_error(char *fmt, ...);

int init_passivesock(char *transport_server, int port);

int init_udp_psock(int port);
int init_tcp_psock(int port);

#endif
