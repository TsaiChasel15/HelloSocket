#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

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

enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};

struct DataHeader {
	short DataLength;
	short Cmd;
};

struct Login : public DataHeader {
	Login() {
		DataLength = sizeof(Login);
		Cmd = CMD_LOGIN;
	}
	char UserName[32];
	char PassWord[32];
};

struct LoginResult : public DataHeader {
	LoginResult() {
		DataLength = sizeof(LoginResult);
		Cmd = CMD_LOGIN_RESULT;
		Result = 0;
	}
	int Result;
};

struct Logout : public DataHeader {
	Logout() {
		DataLength = sizeof(Logout);
		Cmd = CMD_LOGOUT;
	}
	char UserName[32];
};

struct LogoutResult : public DataHeader {
	LogoutResult() {
		DataLength = sizeof(LogoutResult);
		Cmd = CMD_LOGOUT_RESULT;
		Result = 0;
	}
	int Result;
};

void SetColor(ForegroundColor foreColor, BackGroundColor backColor);


int main() {
	//����Windows socket 2.x����
	WORD Version = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(Version, &wsadata);

	//-- ��Socket API�������׵�TCP�ͻ���
	// 1 ����һ��socket
	SOCKET SockCli = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (INVALID_SOCKET == SockCli) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- ����SOCKETʧ�� ��                      " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Cyan);
		std::cout << " --- ����SOCKET�ɹ� ��                      " << std::endl;
	}

	// 2 ���ӷ����� connect
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int nAddrLen = sizeof(sockaddr_in);
	int ret = connect(SockCli, (sockaddr*)&addrSer, nAddrLen);
	if (SOCKET_ERROR == ret) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- ���ӷ�����ʧ�� ��                      " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Green);
		std::cout << " --- ���ӷ������ɹ� ��                      " << std::endl;
	}

	while (1) {
		// 3 ������������
		char CmdBuf[128] = {};
		SetColor(enmCFC_HighWhite, enmCBC_Black);
		std::cin >> CmdBuf;
		// 4 ������������
		if (0 == strcmp(CmdBuf, "exit")) {
			SetColor(enmCFC_Red, enmCBC_Yellow);
			std::cout << " --- Client ׼���˳�                        " << std::endl;
			break;
		}
		else if (0 == strcmp(CmdBuf, "login")) {
			// 5 ���������������
			Login login;
			login.Cmd = CMD_LOGIN;
			login.DataLength = sizeof(login);
			strcpy(login.UserName, "Tsai");
			strcpy(login.PassWord, "MyPassword");
			send(SockCli, (const char*)&login, sizeof(login), 0);
			
			// 6 ���շ�������Ϣ recv
			LoginResult loginret;
			int nLen;
			nLen = recv(SockCli, (char*)&loginret, sizeof(loginret), 0);
			SetColor(enmCFC_HighWhite, enmCBC_Black);
			std::cout << " --- Server ��Ӧ �� LoginResult : " << loginret.Result << std::endl;
		}
		else if (0 == strcmp(CmdBuf, "logout")) {
			// 5 ���������������
			Logout logout;
			logout.Cmd = CMD_LOGOUT;
			logout.DataLength = sizeof(logout);
			strcpy(logout.UserName, "Tsai");
			send(SockCli, (const char*)&logout, sizeof(logout), 0);
			
			// 6 ���շ�������Ϣ recv
			LogoutResult logoutret;
			int nLen;
			nLen = recv(SockCli, (char*)&logoutret, sizeof(logoutret), 0);
			SetColor(enmCFC_HighWhite, enmCBC_Black);
			std::cout << " --- Server ��Ӧ �� LogoutResult : " << logoutret.Result << std::endl;
		}
		else {
			SetColor(enmCFC_Black, enmCBC_Red);
			std::cout << " --- Client �������֧��                  " << std::endl;
		}
	}

	// 4 �ر����ӽ�
	closesocket(SockCli);
	SetColor(enmCFC_Red, enmCBC_Yellow);
	std::cout << " --- Client �Ѿ�����                        " << std::endl;
	getchar();
	WSACleanup();
	return 0;
}


void SetColor(ForegroundColor foreColor, BackGroundColor backColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foreColor | backColor);
}