/*
로컬 호스트 주소에 서버를 생성 / 복수의 클라이언트를 받은 후에
클라이언트가 받은 메시지를 다른 클라언트들에게 전송하는 기능을 가지고 있습니다.
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <process.h>
#pragma comment(lib,"wsock32.lib")

#define PORT 5000			// 사용할 포트는 5000
#define IP "127.0.0.1"		// 접속할 서버는 로컬 호스트


void recv_thread(void*);	// 스레드 수행 함수의 프로토 타입
int ret = 0;				// 리턴 값
SOCKET s = 0;					// 소켓 값
char select_in[2];
char log[10];
HANDLE hMutex;				// 뮤텍스용
char i;              // 비교용
int j;

int main()
{

	while (TRUE)
	{
		// 뮤택스 초기화
		hMutex = CreateMutex(NULL, FALSE, FALSE);
		if (!hMutex)

		{
			printf("Mutex 생성 오류\n");
			return 1;

		}

		// 윈속 초기화
		WSADATA wsd;
		char buff[1024];
		if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
		{
			printf("Winsock 오류\n");
			return 1;
		}

		// 서버로 접속하기 위한 소켓 생성
		sockaddr_in server;

		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == SOCKET_ERROR)
		{
			printf("socket() 오류\n");
			closesocket(s);
			WSACleanup();
			return 1;

		}

		server.sin_addr.s_addr = inet_addr(IP);
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);

		// 서버와 연결
		if (connect(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			printf("connect() 오류\n");
			closesocket(s);
			WSACleanup();
			return 1;

		}
		printf("\n");
		printf("서버와 연결이 되었습니다.\n");

		while (TRUE)
		{
			printf("*회원가입을 원하시는 경우 : 1\n* 로그인 을 원하시는 경우 : 2\n*프로그램 종료를 원하시면 : 3 를 입력하시오\n");
			memset(select_in, 0, sizeof(select_in));
			gets(select_in);
			printf("*** %s 을 입력하셨습니다. ***\n", select_in);
			send(s, select_in, strlen(select_in), 0);
			if (strcmp(select_in, "3") == 0)
			{
				printf("프로그램 종료\n");
				//closesocket(s);
				return 0;			// 접속을 종료
			}
			if (strcmp(select_in, "1") == 0)
			{

				char log[8];
				int nSend;
				printf("+-------------------------------------------------------+\n");
				printf("+             회원가입창을 선택 하셨습니다.             +\n");
				printf("+                                                       +\n");
				printf("+           생성하실 ID를 입력 하여 주십시오.           +\n");
				printf("+-------------------------------------------------------+\n");
				printf("ID : ");
				gets(log);
				nSend = send(s, log, strlen(log), 0);
				recv(s, select_in, sizeof(log), 0);
				if (strcmp(select_in, "1") == 0)
				{
					printf("ID를 생성하였습니다.(%s)\n", log);
				}
				else
				{
					printf("이미 회원 가입 되어있는 ID 입니다.(%s)\n", log);
				}
			}
			if (strcmp(select_in, "2") == 0)
			{

				int nSend;
				printf("+-------------------------------------------------------+\n");
				printf("+               로그인창을 선택 하셨습니다.             +\n");
				printf("+                                                       +\n");
				printf("+               ID를 입력 하여 주십시오.                +\n");
				printf("+-------------------------------------------------------+\n");
				memset(log, 0, sizeof(log));
				gets(log);
				nSend = send(s, log, strlen(log), 0);
				memset(log, 0, sizeof(log));
				recv(s, log, sizeof(log), 0);
				if (strcmp(log, "0") == 0)
				{
					printf("+-------------------------------------------------------+\n");
					printf("+                   로그인 되었습니다.                  +\n");
					printf("+        로그아웃을 원하실 경우 @@ 를 입력하시오.       +\n");
					printf("+-------------------------------------------------------+\n");
					break;


				}
				if (strcmp(log, "7") == 0)
				{
					printf("+--------------------------------------------------------+\n");
					printf("+               ID를 잘못 입력하셨습니다.                +\n");
					printf("+                초기화면으로 돌아갑니다.                +\n");
					printf("+--------------------------------------------------------+\n");
				}
			}


		}
		// 서버에서 우선 주는 Welcome 메시지 분석
		ret = recv(s, buff, 1024, 0);
		printf("%s", buff);

		// 가득 찼다고 메시지가 왔다면
		if (!strcmp("가득찼습니다.\n", buff))
		{
			closesocket(s);
			WSACleanup();
			return 0;			// 접속을 종료
		}

		// 정상 접속이 되면 스레드 작동 - 받는 메시지를 스레드로 실시간 수행
		_beginthread(recv_thread, 0, NULL);

		// 보내는 메시지는 스레드로 넣을 필요가 없습니다.
		while (ret != INVALID_SOCKET || ret != SOCKET_ERROR)
		{

			// 딜레이 : CPU 점유율 감소용
			Sleep(10);
			printf("보낼 메시지 : ");
			gets(buff);

			// 전송 결과 잘못 된 결과를 얻었을때 탈출
			if (ret == INVALID_SOCKET || ret == SOCKET_ERROR) break;


			// 서버로 보내는 글은 fgets 을 받고 나서 순차적으로 보냄
			ret = send(s, buff, strlen(buff), 0);
			if (strcmp(buff, "@@") == 0)
			{
				break;
			}

			// 버퍼 초기화
			memset(buff, 0, 1024);

		}

		// 정상/비정상으로 서버와 통신 결과값이 틀린 값을 받았을 때
		printf("로그아웃이 정상적으로 됬습니다.\n");
		closesocket(s);
		WSACleanup();

	}
}
// 받는 스레드 부분
void recv_thread(void* pData)
{
	int ret_thread = 65535;
	char buff_thread[1024] = { 0 };
	char ID[8] = { 0 };

	// 스레드용 리턴 값이 우너하는 값이 아니면 받는 중에 서버와 통신이 끊겼다고 보고 나감
	while ((ret_thread != INVALID_SOCKET) || (ret_thread != SOCKET_ERROR))
	{
		Sleep(10);	// CPU 점유률 100% 방지용

		// 서버에서 주는 메시지를 실시간으로 기다렸다가 받습니다.
		ret_thread = recv(s, buff_thread, sizeof(buff_thread), 0);
		///recv(s, ID, sizeof(ID), 0);

		// 서버에서 받는 작업을 한 결과 비정상일 때 탈출
		if ((ret_thread == INVALID_SOCKET) || (ret_thread == SOCKET_ERROR))
		{
			break;
		}
		// 정상적으로 받은 버퍼를 출력
		printf("\n받은 메시지: %s\n", buff_thread);
		memset(buff_thread, 0, 1024);	// 받은 버퍼를 초기와
		printf("보낼 메시지: ");
	}
	// 작업이 끝난 소켓을 무효화시킴
	WaitForSingleObject(hMutex, 100L);
	ret = INVALID_SOCKET;
	ReleaseMutex(hMutex);

	return;
}
