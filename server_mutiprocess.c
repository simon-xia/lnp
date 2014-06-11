 /* 
 *   server with muti-process 
 *				Fri Mar 21 21:25:10 CST 2014
 *              by  Simon Xia
 */
#include"simon_socket.h"

#define SERV_PORT 12345

extern pid_t waitpid(pid_t, int *, int);

void handler(int signo)
{
	pid_t pid;
	int status;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		printf("Child process %d terminated\nThe WEXITSTATUS return code is %d \nThe WIFEXITED return code is %d\n", pid, WEXITSTATUS(status), WIFEXITED(status));
	}
}

int main()
{
	int sockfd, acfd;
	struct sockaddr_in client_addr;

	size_t sin_len = sizeof(client_addr);

	sockfd = init_tcp_psock(SERV_PORT);

	signal(SIGCHLD, handler);

/*   another method :
 *
	struct sigaction signala;
	signala.sa_handler = SIG_IGN;
	signala.sa_flags = SA_NOCLDWAIT;
	sigemptyset(&signala.sa_mask);
	sigaction(SIGCHLD, &signala, NULL);
*/	
	while (1)
	{
		if ((acfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_len)) == -1)
		{
			perror("Accept request failed: ");
			return 1;
		}
		else
			printf("Get a connection from %s:%d !\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		pid_t pid;
		if ((pid = fork()) > 0)
		{
			close(acfd);
			continue;
		}
		else if(pid == 0)
		{
			close(sockfd);
			process_client(acfd, &client_addr);
			close(acfd);
			exit(0); //MARK
		}
		else
		{
			perror("Fork error");
			exit(0);
		}
	}
	close(sockfd);
	return 0;
}
