#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT  5000

#define TRUE             1
#define FALSE            0
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

fd_set master_set;
int listen_sd;
int max_sd;
void init_set()
{
	FD_ZERO(&master_set);
	max_sd=0;
}

void add_to_set(int new_sd)
{
	FD_SET(new_sd, &master_set);
	max_sd=MAX(new_sd, max_sd);
}

void remove_from_set(int sd)
{
	FD_CLR(sd, &master_set);
	if (sd == max_sd)
	{
		while (FD_ISSET(max_sd, &master_set) == FALSE)
			max_sd -= 1;
	}
}

void broadcast(int sender, char* msg)
{
	int j;
	int len=strlen(msg);
	for (j=0; j <= max_sd  ; ++j)
	{
		if (FD_ISSET(j, &master_set) && j!=listen_sd && j!=sender)
		{
			send(j, msg, len, 0);
		}
	}
	printf("Broadcasting '%s' finished\n" , msg);
}
// see http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzab6%2Frzab6xnonblock.htm
// for detailed explanations
int main (int argc, char *argv[])
{
	int    i, j, len, rc, on = 1;
	int    new_sd;
	int    close_conn;
	char   buffer[4096];
	char   buffer2[4096];
	struct sockaddr_in   addr;
	struct timeval       timeout;
	fd_set select_result;

	listen_sd = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_sd < 0) exit(-1);

	/* Allow socket descriptor to be reuseable                   */
	rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0) exit(-2);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port        = htons(SERVER_PORT);
	rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) exit(-4);

	rc = listen(listen_sd, 32);
	if (rc < 0) exit(-5);

	printf("Chat server started on port %d\n", SERVER_PORT);

	init_set();
	add_to_set(listen_sd);

	/* Initialize the timeval struct to 3 minutes.          */
	timeout.tv_sec  = 3 * 60;
	timeout.tv_usec = 0;

	while(TRUE)
	{
		memcpy(&select_result, &master_set, sizeof(master_set));

		printf("Waiting on select()...\n");
		rc = select(max_sd + 1, &select_result, NULL, NULL, &timeout);

		if (rc < 0) exit(-6); // select failed
		if (rc == 0) { printf("timeout\n"); exit(-7); } // timeout

		// rc descriptors are readable now.  
		for (i=0; i <= max_sd  ; ++i)
		{
			if (FD_ISSET(i, &select_result))
			{
				if (i == listen_sd)
				{
					new_sd = accept(listen_sd, NULL, NULL);
					if (new_sd < 0) exit(-8);

					printf("Client %d connected\n", new_sd);
					add_to_set(new_sd);
					snprintf(buffer, 4096, "[%d] entered room", new_sd);
					broadcast(new_sd, buffer);
				}
				else
				{
					rc = recv(i, buffer, 4096, 0);
					if (rc < 0) exit(-9); // recv failed
					if (rc == 0)  // disconnected
					{
						close(i);
						remove_from_set(i);
						snprintf(buffer2, 4096, "Client %d disconnected\n", i);
					}
					else
					{
						buffer[rc]=0; // make null terminated string
						snprintf(buffer2, 4096, "<%d> %s", i, buffer);
					}

					// broad cast to other clients
					broadcast(i, buffer2);
				} 
			} 
		} 
	} 

	for (i=0; i <= max_sd; ++i)
	{
		if (FD_ISSET(i, &master_set))
			close(i);
	}
}

