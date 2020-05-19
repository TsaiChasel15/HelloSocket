#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include<iomanip>
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
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader {
	short DataLength;
	short Cmd;
};

struct Login : public DataHeader{
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

struct NewUserJion : public DataHeader {
	NewUserJion() {
		DataLength = sizeof(NewUserJion);
		Cmd = CMD_NEW_USER_JOIN;
		SockID = 0;
	}
	int SockID;
};

std::vector<SOCKET> g_Clients;

void SetColor(ForegroundColor foreColor, BackGroundColor backColor);

int ServerProcessor(SOCKET Client) {
	// 5 recv ���տͻ�������
	//������ ��ע��recv������һֱ����ֱ������ֵΪ < 0������recv���
	char szRecv[1024] = {};
	int RecvLen = recv(Client, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (RecvLen <= 0) {
		SetColor(enmCFC_Black, enmCBC_HighWhite);
		std::cout << " --- �յ�Client Socket = " << std::setw(4) << std::setfill('0') << Client << " �Ѿ��˳�              " << std::endl;
		return -1;
	}

	// 6 send ����ͻ�������
	switch (header->Cmd) {
	case CMD_LOGIN:
	{
		recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
		Login* login = (Login*)szRecv;
		SetColor(enmCFC_HighWhite, enmCBC_Black);
		std::cout << " --- �յ�Client Socket = " << std::setw(4) << std::setfill('0') << Client << " ���� CMD_LOGIN ��" << login->Cmd
			<< " ���� ��" << login->DataLength << " UserName ��" << login->UserName
			<< " PassWord ��" << login->PassWord << std::endl;
		LoginResult ret;
		ret.Result = 1;
		send(Client, (char*)&ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
		Logout* logout = (Logout*)szRecv;
		SetColor(enmCFC_HighWhite, enmCBC_Black);
		std::cout << " --- �յ�Client Socket = " << std::setw(4) << std::setfill('0') << Client << " ���� CMD_LOGOUT ��" << logout->Cmd
			<< " ���� ��" << logout->DataLength << " UserName ��" << logout->UserName << std::endl;
		LoginResult ret;
		ret.Result = 1;
		send(Client, (char*)&ret, sizeof(LogoutResult), 0);
	}
	break;
	default:
	{
		DataHeader header{ 0, CMD_ERROR };
		SetColor(enmCFC_HighWhite, enmCBC_Black);
		std::cout << " --- Client ���� CMD ��δ֪����" << std::endl;
		send(Client, (char*)&header, sizeof(DataHeader), 0);
	}
	break;
	}
}

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
		std::cout << " --- ����SOCKETʧ��                                 " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Cyan);
		std::cout << " --- ����SOCKET�ɹ�                                 " << std::endl;
	}

	// 2 bind �����ڽ��տͻ������ӵ�����˿�
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = INADDR_ANY ;//inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(SockSer, (sockaddr*)&addrSer, sizeof(addrSer))) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- �󶨶˿�ʧ��                                   " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Green);
		std::cout << " --- �󶨶˿ڳɹ�                                   " << std::endl;
	}
	
	// 3 listen ��������˿�
	if(SOCKET_ERROR == listen(SockSer, 5)) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- �����˿�ʧ��                                   " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Cyan);
		std::cout << " --- �����˿ڳɹ�                                   " << std::endl;
	}
	
	
	int maxfd = SockSer + 1;
	while (1) {
		//select׼������
		//a.���弯��(����������)
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		//b.��ռ���
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		//c.��ӵ�����(��SockSer��ӵ�������)
		FD_SET(SockSer, &fdRead);
		FD_SET(SockSer, &fdWrite);
		FD_SET(SockSer, &fdExp);

		//c.��ӵ�����(�����ص�Client Socket��ӵ�������)
		for (size_t n = 0; n < g_Clients.size(); n++) {
			FD_SET(g_Clients[n], &fdRead);
		}

		// 4 select��ѯ�ж��Ƿ����¿ͻ������ӻ��߿ͻ�������
		timeval t{1,0};
		int ret = select(maxfd, &fdRead, &fdWrite, &fdExp, &t);//���һ������Ϊnullptr����, ����timeval��ֵ���ȴ�timevalʱ�䷵��
		if (ret < 0) {
			SetColor(enmCFC_Red, enmCBC_Yellow);
			std::cout << " --- Client select ����                              " << std::endl;
			break;
		}

		// 5 accpet �ȴ����տͻ�������
		//�ж�SockSer�Ƿ����ڼ�������  ���� �ж��Ƿ����¿ͻ��˷�������
		if (FD_ISSET(SockSer, &fdRead)) {
			FD_CLR(SockSer, &fdRead);
			
			SOCKET Client = INVALID_SOCKET;
			sockaddr_in ClientAddr{};
			int nAddrLen = sizeof(sockaddr_in);
			Client = accept(SockSer, (sockaddr*)&ClientAddr, &nAddrLen);
			if (INVALID_SOCKET == Client) {
				SetColor(enmCFC_Black, enmCBC_Red);
				std::cout << " --- ��Ч�Ŀͻ�������                               " << std::endl;
			}
			else {
				// ֪ͨ���пͻ������µĿͻ��˼���
				NewUserJion userJoin;
				userJoin.SockID = Client;
				for (size_t n = 0; n < g_Clients.size(); n++) {
					send(g_Clients[n], (const char*)&userJoin, sizeof(NewUserJion), 0);
				}
				g_Clients.push_back(Client);
				if (maxfd <= Client)
					maxfd = Client + 1;

				SetColor(enmCFC_Black, enmCBC_Blue);
				std::cout << " --- �յ�Client Socket = " << std::setw(4) << std::setfill('0') << Client << " IP = "
					<< inet_ntoa(ClientAddr.sin_addr) << "        " << std::endl;
			}
		}

		// 6 ����Client��������Ϣ����
		for (size_t n = 0; n < fdRead.fd_count; n++) {
			if (-1 == ServerProcessor(fdRead.fd_array[n])) {
				auto iter = find(g_Clients.begin(), g_Clients.end(), fdRead.fd_array[n]);
				if (iter != g_Clients.end()) {
					g_Clients.erase(iter);
				}
			}
		}

		SetColor(enmCFC_Black, enmCBC_Yellow);
		std::cout << " --- ����Server Socket = " << std::setw(4) << std::setfill('0') 
			<< SockSer << " �����Լ�������        " << std::endl;
	}

	for (size_t n = 0; n < g_Clients.size(); n++) {
		closesocket(g_Clients[n]);
	}

	// 6 �ر����ӽ�
	closesocket(SockSer);
	SetColor(enmCFC_Red, enmCBC_Yellow);
	std::cout << " --- Server �Ѿ�����                                " << std::endl;
	SetColor(enmCFC_HighWhite, enmCBC_Black);
	getchar();
	WSACleanup();
	return 0;
}

void SetColor(ForegroundColor foreColor, BackGroundColor backColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foreColor | backColor);
}