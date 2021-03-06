#ifndef _SIMON_SOCKET_H_
#define _SIMON_SOCKET_H_

#include<stdarg.h> // va_list etc
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>      //for fork open read etc
#include<pthread.h>
#include<signal.h>       
#include<pthread.h>
#include<sys/socket.h>   // for socket, bind, listen, setsockopt
#include<sys/types.h>
#include<netinet/in.h>   //sockaddr_in
#include<arpa/inet.h>    //htons etc
#include<netdb.h>    //gethostbyname etc

#include<errno.h>   // for perror, errno

#define MAX_BACKLOG 10 //default 20
#define MAX_BUF_SIZE 1024

extern int errno;
char send_buf[MAX_BUF_SIZE+1], recv_buf[MAX_BUF_SIZE+1];

void exit_error(char *fmt, ...);

int init_passivesock(char *transport_server, int port);

int init_udp_psock(int port);
int init_tcp_psock(int port);

int simon_send(int fd, char*buf, unsigned int n);
int simon_recv(int fd, char*buf, unsigned int n);

int init_initiativesock(char *transport_server, char *addr, int port);
int init_udp_isock();
int init_tcp_isock(char *addr, int port);

void convert_upper(char * buf, int len);
void process_client(int connectfd, const struct sockaddr_in *client);

int accept_request(int, struct sockaddr*, size_t *);


#endif
