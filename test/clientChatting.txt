#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                 //이게 안된다고 뜨네요.
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
		printf("클라이언트 소켓을 생성하지 못했습니다.\n");
		return 0;
	}
	printf("클라이언트 소켓을 생성했습니다.\n");
	sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_addr.s_addr=inet_addr("127.0.0.1");
	serverAddress.sin_family=AF_INET;
	serverAddress.sin_port=htons(PORT);

	if(connect(mySocket, (sockaddr*) &serverAddress, sizeof(serverAddress))==-1)
	{
		printf("서버와 연결하는데 실패했습니다. \n");
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
		printf("채팅을 시작합니다.\n");
		printf("클라이언트의 대화명을 입력하세요-->");
		fgets(myName, NAMESIZE, stdin);
		myName[strlen(myName)-1]='\0';
		sprintf(toServer,"[%s] 님이 입장하셨습니다.", myName);
		write(mySocket, toServer, sizeof(toServer));

		while(1)
		{
			fgets(fromClientInput, BUFFERSIZE, stdin);
			sprintf(toServer, "[%s] %s", myName,fromClientInput);
			toServer [strlen(toServer)-1]='\0';
			write(mySocket, toServer, sizeof(toServer));

			if(strcmp(strlwr(fromClientInput),"quit\n")==0)
			{
				sprintf(toServer,"[%s} 님이 나가셨습니다." myName);
				write(mySocket, toServer, sizeof(toServer));
				printf("프로그램을 종료합니다.\n");
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