#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                 //�̰� �ȵȴٰ� �߳׿�.
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 5500
#define BUFFERSIZE 100
#define NAMESIZE 30

void *sendMessage (void* arg);
void *receiveMessage (void *arg);

pthread_t sendThread, receiveThread;

int main()
{
	int mySocket = socket ( PF_INET, SOCK_STREAM, 0);
	
	if (mySocket == -1)
	{
		printf("Ŭ���̾�Ʈ ������ �������� ���߽��ϴ�.\n");
		return 0;
	}
	printf("Ŭ���̾�Ʈ ������ �����߽��ϴ�.\n");
	sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_addr.s_addr=inet_addr("127.0.0.1");
	serverAddress.sin_family=AF_INET;
	serverAddress.sin_port=htons(PORT);

	if(connect(mySocket, (sockaddr*) &serverAddress, sizeof(serverAddress))==-1)
	{
		printf("������ �����ϴµ� �����߽��ϴ�. \n");
		return 0;
	}

	char greetMessage [BUFFERSIZE];
	read(mySocket, greetMessage, sizeof(greetMessage));
	printf("%s \n", greetMessage);
	pthread_create (&sendThread, NULL, sendMessage, (void*) mySocket);
	pthread_create (&receiveThread, NULL, receiveMessage, (void*)mySocket);
	pthread_join(sendThread, NULL);
	pthread_join (reveiceThread, NULL);

	void *sendMessage(void* arg)
	{
		int mySocket=(int) arg;
		char fromClientInput[BUFFERSIZE]
		char myName[NAMESIZE};
		char toServer[NAMESIZE+BUFFERSIZE];
		printf("ä���� �����մϴ�.\n");
		printf("Ŭ���̾�Ʈ�� ��ȭ���� �Է��ϼ���-->");
		fgets(myName, NAMESIZE, stdin);
		myName[strlen(myName)-1]='\0';
		sprintf(toServer,"[%s] ���� �����ϼ̽��ϴ�.", myName);
		write(mySocket, toServer, sizeof(toServer));

		while(1)
		{
			fgets(fromClientInput, BUFFERSIZE, stdin);
			sprintf(toServer, "[%s] %s", myName,fromClientInput);
			toServer [strlen(toServer)-1]='\0';
			write(mySocket, toServer, sizeof(toServer));

			if(strcmp(strlwr(fromClientInput),"quit\n")==0)
			{
				sprintf(toServer,"[%s} ���� �����̽��ϴ�." myName);
				write(mySocket, toServer, sizeof(toServer));
				printf("���α׷��� �����մϴ�.\n");
				close(mySocket);
				break;
			}
		}
	}

	void *receiveMessage (void *arg)
	{
		int mySocket=(int) arg;
		int strlen=0;
		char fromServer[NAMESIZE+BUFFERSIZE];

		while(1)
		{
			strlen=read(mySocket, fromServer, sizeof(fromServer));
			
			if(strlen<=0)
			{
				break;
			}
			printf("%s\n",fromServer);
		}
	}
}