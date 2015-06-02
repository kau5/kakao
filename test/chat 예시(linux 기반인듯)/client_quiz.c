#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT  5000
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

void prompt() 
{
	printf("<You> ");
	fflush(stdout);
}
main (int argc, char *argv[])
{
	int    len, rc;
	int    sockfd;
	char   send_buf[4096];
	char   recv_buf[4096];
	struct sockaddr_in   serv_addr;
	struct timeval       timeout;
	fd_set master_set, select_result;
	int max_sd;


	if(argc!=2) exit(1);// incorrect usage

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0)
		exit(2); // socket creation error

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_port        = htons(SERVER_PORT);

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
		exit(3); // ip adress error

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0)
		exit(4); // connection error

	FD_ZERO(&master_set);
	FD_SET(fileno(stdin), & master_set);
	FD_SET(sockfd, & master_set);
	max_sd=MAX(fileno(stdin), sockfd);

	/* Initialize the timeval struct to 3 minutes.          */
	timeout.tv_sec  = 3 * 60;
	timeout.tv_usec = 0;

	printf("Connected to remote host. Start sending messages.\n");
	prompt();

	while(1)
	{
		----------------QUIZ 1--------------------

		if(--------QUIZ 2-------)
		{
			gets(send_buf);
			send(sockfd, send_buf, strlen(send_buf), 0);
			prompt();
		}
		else if(----QUIZ 3----)
		{
			rc = recv(sockfd, recv_buf, 4096, 0);
			if (rc < 0) exit(-9); // recv failed
			if (rc == 0) 
			{
				printf("Server disconnected\n");
				close(sockfd);
				exit(0);
			}
			recv_buf[rc]=0; // make null terminated string
			printf("\r%s\n", recv_buf);
			prompt();
		}
		else
		{
			// timed out
			close(sockfd);
			return 0;
		}
	}
	close(sockfd);
	return 0;
}
