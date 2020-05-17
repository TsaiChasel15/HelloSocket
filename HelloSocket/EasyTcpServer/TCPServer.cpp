#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")



enum ForegroundColor
{
	enmCFC_Red = FOREGROUND_INTENSITY | FOREGROUND_RED,
	enmCFC_Green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	enmCFC_Blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	enmCFC_Yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	enmCFC_Purple = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	enmCFC_Cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Gray = FOREGROUND_INTENSITY,
	enmCFC_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_HighWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Black = 0,
};

enum BackGroundColor
{
	enmCBC_Red = BACKGROUND_INTENSITY | BACKGROUND_RED,
	enmCBC_Green = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
	enmCBC_Blue = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
	enmCBC_Yellow = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
	enmCBC_Purple = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
	enmCBC_Cyan = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_White = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_HighWhite = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_Black = 0,
};

void SetColor(ForegroundColor foreColor, BackGroundColor backColor);

int main() {
	//����Windows socket 2.x����
	WORD Version = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(Version, &wsadata);

	//-- ��Socket API�������׵�TCP������
	// 1 ����һ��socket
	SOCKET SockSer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == SockSer) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- ����SOCKETʧ��                         " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Cyan);
		std::cout << " --- ����SOCKET�ɹ�                         " << std::endl;
	}

	// 2 bind �����ڽ��տͻ������ӵ�����˿�
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = INADDR_ANY ;//inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(SockSer, (sockaddr*)&addrSer, sizeof(addrSer))) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- �󶨶˿�ʧ��                           " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Green);
		std::cout << " --- �󶨶˿ڳɹ�                           " << std::endl;
	}
	
	// 3 listen ��������˿�
	if(SOCKET_ERROR == listen(SockSer, 5)) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- �����˿�ʧ��                           " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Cyan);
		std::cout << " --- �����˿ڳɹ�                           " << std::endl;
	}
	
	// 4 accpet �ȴ����տͻ�������
	sockaddr_in ClientAddr{};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET Client = INVALID_SOCKET;

	Client = accept(SockSer, (sockaddr*)&ClientAddr, &nAddrLen);
	if (INVALID_SOCKET == Client) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- ��Ч�Ŀͻ�������                       " << std::endl;
	}
	SetColor(enmCFC_Black, enmCBC_Green);
	std::cout << " --- �¿ͻ���  Socket = " << Client <<  "   IP = " << inet_ntoa(ClientAddr.sin_addr) << std::endl;

	char RecvBuf[128] = {};

	// 5 send ��ͻ��˷���һ����Ϣ
	while (1) {
		int RecvLen = recv(Client, RecvBuf, 128, 0);
		if (RecvLen <= 0) {
			SetColor(enmCFC_Red, enmCBC_Yellow);
			std::cout << " --- Client ׼���˳�                        " << std::endl;
			break;
		}
		SetColor(enmCFC_HighWhite, enmCBC_Black);
		std::cout << " --- Client ���� ��" << RecvBuf << std::endl;

		if (0 == strcmp(RecvBuf, "getName")) {
			char SendBuf[] = "I am Server~";
			send(Client, SendBuf, sizeof(SendBuf) + 1, 0);
		}
		else if (0 == strcmp(RecvBuf, "getAge")) {
			char SendBuf[] = "I am 80";
			send(Client, SendBuf, sizeof(SendBuf) + 1, 0);
		}
		else {
			char SendBuf[] = "?????";
			send(Client, SendBuf, sizeof(SendBuf) + 1, 0);
		}
	}

	// 6 �ر����ӽ�
	closesocket(SockSer);
	SetColor(enmCFC_Red, enmCBC_Yellow);
	std::cout << " --- Server �Ѿ�����                        " << std::endl;
	getchar();
	WSACleanup();
	return 0;
}

void SetColor(ForegroundColor foreColor, BackGroundColor backColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foreColor | backColor);
}