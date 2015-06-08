/*
���� ȣ��Ʈ �ּҿ� ������ ���� / ������ Ŭ���̾�Ʈ�� ���� �Ŀ�
Ŭ���̾�Ʈ�� ���� �޽����� �ٸ� Ŭ���Ʈ�鿡�� �����ϴ� ����� ������ �ֽ��ϴ�.
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <process.h>
#pragma comment(lib,"wsock32.lib")

#define PORT 5000			// ����� ��Ʈ�� 5000
#define IP "127.0.0.1"		// ������ ������ ���� ȣ��Ʈ


void recv_thread(void*);	// ������ ���� �Լ��� ������ Ÿ��
int ret = 0;				// ���� ��
SOCKET s = 0;					// ���� ��
char select_in[2];
char log[10];
HANDLE hMutex;				// ���ؽ���
char i;              // �񱳿�
int j;

int main()
{

	while (TRUE)
	{
		// ���ý� �ʱ�ȭ
		hMutex = CreateMutex(NULL, FALSE, FALSE);
		if (!hMutex)

		{
			printf("Mutex ���� ����\n");
			return 1;

		}

		// ���� �ʱ�ȭ
		WSADATA wsd;
		char buff[1024];
		if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
		{
			printf("Winsock ����\n");
			return 1;
		}

		// ������ �����ϱ� ���� ���� ����
		sockaddr_in server;

		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == SOCKET_ERROR)
		{
			printf("socket() ����\n");
			closesocket(s);
			WSACleanup();
			return 1;

		}

		server.sin_addr.s_addr = inet_addr(IP);
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);

		// ������ ����
		if (connect(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			printf("connect() ����\n");
			closesocket(s);
			WSACleanup();
			return 1;

		}
		printf("\n");
		printf("������ ������ �Ǿ����ϴ�.\n");

		while (TRUE)
		{
			printf("*ȸ�������� ���Ͻô� ��� : 1\n* �α��� �� ���Ͻô� ��� : 2\n*���α׷� ���Ḧ ���Ͻø� : 3 �� �Է��Ͻÿ�\n");
			memset(select_in, 0, sizeof(select_in));
			gets(select_in);
			printf("*** %s �� �Է��ϼ̽��ϴ�. ***\n", select_in);
			send(s, select_in, strlen(select_in), 0);
			if (strcmp(select_in, "3") == 0)
			{
				printf("���α׷� ����\n");
				//closesocket(s);
				return 0;			// ������ ����
			}
			if (strcmp(select_in, "1") == 0)
			{

				char log[8];
				int nSend;
				printf("+-------------------------------------------------------+\n");
				printf("+             ȸ������â�� ���� �ϼ̽��ϴ�.             +\n");
				printf("+                                                       +\n");
				printf("+           �����Ͻ� ID�� �Է� �Ͽ� �ֽʽÿ�.           +\n");
				printf("+-------------------------------------------------------+\n");
				printf("ID : ");
				gets(log);
				nSend = send(s, log, strlen(log), 0);
				recv(s, select_in, sizeof(log), 0);
				if (strcmp(select_in, "1") == 0)
				{
					printf("ID�� �����Ͽ����ϴ�.(%s)\n", log);
				}
				else
				{
					printf("�̹� ȸ�� ���� �Ǿ��ִ� ID �Դϴ�.(%s)\n", log);
				}
			}
			if (strcmp(select_in, "2") == 0)
			{

				int nSend;
				printf("+-------------------------------------------------------+\n");
				printf("+               �α���â�� ���� �ϼ̽��ϴ�.             +\n");
				printf("+                                                       +\n");
				printf("+               ID�� �Է� �Ͽ� �ֽʽÿ�.                +\n");
				printf("+-------------------------------------------------------+\n");
				memset(log, 0, sizeof(log));
				gets(log);
				nSend = send(s, log, strlen(log), 0);
				memset(log, 0, sizeof(log));
				recv(s, log, sizeof(log), 0);
				if (strcmp(log, "0") == 0)
				{
					printf("+-------------------------------------------------------+\n");
					printf("+                   �α��� �Ǿ����ϴ�.                  +\n");
					printf("+        �α׾ƿ��� ���Ͻ� ��� @@ �� �Է��Ͻÿ�.       +\n");
					printf("+-------------------------------------------------------+\n");
					break;


				}
				if (strcmp(log, "7") == 0)
				{
					printf("+--------------------------------------------------------+\n");
					printf("+               ID�� �߸� �Է��ϼ̽��ϴ�.                +\n");
					printf("+                �ʱ�ȭ������ ���ư��ϴ�.                +\n");
					printf("+--------------------------------------------------------+\n");
				}
			}


		}
		// �������� �켱 �ִ� Welcome �޽��� �м�
		ret = recv(s, buff, 1024, 0);
		printf("%s", buff);

		// ���� á�ٰ� �޽����� �Դٸ�
		if (!strcmp("����á���ϴ�.\n", buff))
		{
			closesocket(s);
			WSACleanup();
			return 0;			// ������ ����
		}

		// ���� ������ �Ǹ� ������ �۵� - �޴� �޽����� ������� �ǽð� ����
		_beginthread(recv_thread, 0, NULL);

		// ������ �޽����� ������� ���� �ʿ䰡 �����ϴ�.
		while (ret != INVALID_SOCKET || ret != SOCKET_ERROR)
		{

			// ������ : CPU ������ ���ҿ�
			Sleep(10);
			printf("���� �޽��� : ");
			gets(buff);

			// ���� ��� �߸� �� ����� ������� Ż��
			if (ret == INVALID_SOCKET || ret == SOCKET_ERROR) break;


			// ������ ������ ���� fgets �� �ް� ���� ���������� ����
			ret = send(s, buff, strlen(buff), 0);
			if (strcmp(buff, "@@") == 0)
			{
				break;
			}

			// ���� �ʱ�ȭ
			memset(buff, 0, 1024);

		}

		// ����/���������� ������ ��� ������� Ʋ�� ���� �޾��� ��
		printf("�α׾ƿ��� ���������� ����ϴ�.\n");
		closesocket(s);
		WSACleanup();

	}
}
// �޴� ������ �κ�
void recv_thread(void* pData)
{
	int ret_thread = 65535;
	char buff_thread[1024] = { 0 };
	char ID[8] = { 0 };

	// ������� ���� ���� ����ϴ� ���� �ƴϸ� �޴� �߿� ������ ����� ����ٰ� ���� ����
	while ((ret_thread != INVALID_SOCKET) || (ret_thread != SOCKET_ERROR))
	{
		Sleep(10);	// CPU ������ 100% ������

		// �������� �ִ� �޽����� �ǽð����� ��ٷȴٰ� �޽��ϴ�.
		ret_thread = recv(s, buff_thread, sizeof(buff_thread), 0);
		///recv(s, ID, sizeof(ID), 0);

		// �������� �޴� �۾��� �� ��� �������� �� Ż��
		if ((ret_thread == INVALID_SOCKET) || (ret_thread == SOCKET_ERROR))
		{
			break;
		}
		// ���������� ���� ���۸� ���
		printf("\n���� �޽���: %s\n", buff_thread);
		memset(buff_thread, 0, 1024);	// ���� ���۸� �ʱ��
		printf("���� �޽���: ");
	}
	// �۾��� ���� ������ ��ȿȭ��Ŵ
	WaitForSingleObject(hMutex, 100L);
	ret = INVALID_SOCKET;
	ReleaseMutex(hMutex);

	return;
}
