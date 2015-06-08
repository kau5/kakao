/*
로컬 호스트 주소에 서버를 생성 / 복수의 클라이언트를 받은 후에
클라이언트가 받은 메시지를 다른 클라언트들에게 전송하는 기능을 가지고 있습니다.
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <process.h>
#include <winsock.h>
#include <windows.h>
#include <string.h>

#pragma comment(lib, "wsock32.lib")	
#define PORT 5000			// 사용포트는 5000
#define MAX_CLIENT 5		// 최대 허용 인원 수 5개
#define ALLOW 65535			// 최대 생성 가능 소켓 번호 65535
void ErrorPrint(char *szError);
void login(SOCKET arg);
void recv_client(void *ns);	// 스레드 함수 프로토 타입
int client_num = 0;			// 점유 횟수 (클라이언트 갯수)
int seat = 0;				// 클라언트 번호
int clinet_n = -1;
char welcome_ok[] = "번째 사용자, 환영합니다.\n\0";	// Welcome 정상 초기 글
char welcome_full[] = "가득찼습니다.\n";				// Welcome 사용자 초가시 생기는 글
SOCKET client_sock[ALLOW];		// client_sock (클라이언트 Welcome Socket)
HANDLE hMutex = 0;			// 뮤택스
SOCKET c;
char select_in[10];
char log[8];
char logdata[5][8];
int i = 0;
int j = 0;
int m = 0;
int log_in_out = 0;
char client_name[5][8];
HANDLE hThread;
int addrsize, ret;
sockaddr_in server, client;
SOCKET s;

DWORD WINAPI ProcessClient(LPVOID arg)
{


	SOCKET cs = (SOCKET)arg;
	SOCKADDR_IN caddr;
	int caddrlen;
	caddrlen = sizeof(caddr);
	getpeername(cs, (SOCKADDR *)&caddr, &caddrlen);
	int seat_in = clinet_n;

	while (TRUE){

		memset(select_in, 0, sizeof(select_in));
		ret = recv(cs, select_in, sizeof(select_in), 0);
		if (ret == SOCKET_ERROR || ret == INVALID_SOCKET)
		{
			WaitForSingleObject(hMutex, INFINITE);
			ReleaseMutex(hMutex);
			closesocket(cs);
			return 1;
			printf("1111");
		}
		if (strcmp(select_in, "1") == 0)
		{

			memset(logdata[i], 0, sizeof(logdata[i]));
			recv(cs, logdata[i], sizeof(logdata[i]), 0);

			for (j = 0; j < i; j++)
			{
				if (strcmp(logdata[i], logdata[j]) == 0)
				{
					printf("동일한 ID가 이미 존재합니다.(%s)\n", logdata[j]);
					send(cs, "0", strlen("0"), 0);
					break;
				}
			}
			if (j == i)
			{

				send(cs, "1", strlen("0"), 0);
				printf("ID가 생성되었습니다.(%s)\n", logdata[i]);
				i++;
			}

		}
		else if (strcmp(select_in, "2") == 0)
		{
			int j;
			memset(log, 0, sizeof(log));
			recv(cs, log, sizeof(log), 0);
			for (j = 0; j < 5; j++)
			{
				if (strcmp(log, logdata[j]) == 0)
				{
					printf("로그인 되었습니다.\n");
					break;
				}
			}
			if (j != 5)
			{
				send(cs, "0", sizeof("0"), 0);
				break;

			}
			printf("ID를 다시 입력하여 주십시오.\n");
			send(cs, "7", sizeof("7"), 0);

		}
		memset(client_name[m], 0, sizeof(client_name[m]));
		strcpy(client_name[m], log);
		m++;


	}
	log_in_out = 1;
	client_sock[seat] = cs;
	if (client_num < MAX_CLIENT)
	{

		if (client_sock[seat] != INVALID_SOCKET || client_sock[seat] != SOCKET_ERROR){}
		_beginthread(recv_client, 0, &client_sock[seat]);
		Sleep(10);
		printf("ID : %s가 접속 했습니다.\n", log);

	}

	else	// 가득 찼을 때
	{
		addrsize = sizeof(client);
		if (client_sock[seat] == INVALID_SOCKET)
		{
			printf("accept() 오류\n");
			closesocket(client_sock[seat]);
			closesocket(s);
			WSACleanup();
			return 1;
		}
		ret = send(client_sock[seat], welcome_full, sizeof(welcome_full), 0);
		closesocket(client_sock[seat]);

		// 메시지 보내고 바로 끊는다.

	}

	//받은 데이터 출력
	return 1;
}
int main()
{
	// Welcome Screen
	printf("+---------------------------+\n");
	printf("+ 서버                      +\n");
	printf("+---------------------------+\n");


	// 뮤택스 생성
	hMutex = CreateMutex(NULL, FALSE, NULL);	// 생성 실패시 오류
	if (!hMutex)
	{
		printf("Mutex 오류\n");
		CloseHandle(hMutex);
		return 1;
	}

	// 윈속 초기화
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)	// 사용 소켓 버전은 1.1
	{
		printf("Winsock 오류\n");
		WSACleanup();
		return 1;

	}

	// Listen 소켓 생성


	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == SOCKET_ERROR)
	{
		printf("socket() 오류\n");
		closesocket(s);
		WSACleanup();
		return 1;
	}
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	// Bind 하기
	if (bind(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind() 오류\n");
		closesocket(s);
		WSACleanup();
		return 1;

	}



	printf("사용자를 기다립니다. 이용 가능한 자리 %d개 남았습니다.\n", MAX_CLIENT - client_num);

	listen(s, 10);
	addrsize = sizeof(client);

	while (1)
	{

		c = accept(s, (sockaddr*)&client, &addrsize);
		if (c == INVALID_SOCKET)
		{
			ErrorPrint("accept()");
			break;
		}
		// Blocking 방식으로 Client 를 기다립니다.

		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)c, 0, NULL);
		if (hThread == NULL){
			closesocket(c);
		}

		// accept 시(중요, client_num 가 accept() 함수 수행중 에 변할수 있으므로
		// MAX_CLIENT 도달시랑 따로 accept() 시 문제 발생 가능성 있음


	}
	return 0;

}

void recv_client(void *ns)
{
	// 정상적으로 받아 드릴때, 스레드 실행
	// 클라이언트의 숫자를 늘림
	WaitForSingleObject(hMutex, INFINITE);
	client_num++;				// 클라이언트 갯수 증가
	seat++;							// 클라이언트 번호 증가
	printf("이용 가능한 자리 %d개 남았습니다.\n", MAX_CLIENT - client_num);		// 갯수로 판단

	ReleaseMutex(hMutex);

	char welcome[100] = { 0 };		// accept 된 소켓에게 줄 버퍼 생성
	char buff[1024] = { 0 };
	int ret, i;


	_itoa_s(seat, welcome, 10);			// 클라이언트 번호
	strcat_s(welcome, welcome_ok);		// 정상 환영 메시지 환영
	ret = send(*(SOCKET*)ns, welcome, sizeof(welcome), 0);		// 전송

	while (ret != SOCKET_ERROR || ret != INVALID_SOCKET)
	{
		ret = recv(*(SOCKET*)ns, buff, 1024, 0);		// 클라이언트의 메시지를 받음
		if (strcmp(buff, "@@") == 0)
		{
			memset(buff, 0, 1024);
			break;
		}

		for (i = 0; i < ALLOW; i++)
		{

			// 받은 클라이언트 소켓의 메모리 주소와 보내는 클라이언트 소켓 메모리 주소가 다를때만 전송
			WaitForSingleObject(hMutex, INFINITE);
			if (((unsigned*)&client_sock[i] != (SOCKET*)ns))
			{
				send(client_sock[i], buff, strlen(buff), 0);
			}

			ReleaseMutex(hMutex);


		}

		// 서버 콘솔 창에 전송 기록 남김
		if (strlen(buff) != 0) printf("%d 메시지 보냄 : %s\n", strlen(buff), buff);

		memset(buff, 0, 1024);
		
	}
	// 접속된 소켓이 연결을 해제 시켰을때
	WaitForSingleObject(hMutex, INFINITE);
	client_num--;
	printf("1명의 사용자 로그아웃 \n이용 가능한 자리 %d개 남았습니다\n", MAX_CLIENT - client_num);
	ReleaseMutex(hMutex);

	closesocket(*(int*)ns);

	return;
}
void ErrorPrint(char *szError)
{
	LPVOID lpBuff;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpBuff,
		0,
		NULL);
	printf("%s : %s\n", szError, lpBuff);
	LocalFree(lpBuff);
}