#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

int main() {
	//����Windows socket 2.x����
	WORD Version = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(Version, &wsadata);

	//-- ��Socket API�������׵�TCP������
	// 1 ����һ��socket
	SOCKET SockCli = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (INVALID_SOCKET == SockCli)
		std::cout << "����SOCKETʧ��" << std::endl;
	else
		std::cout << "����SOCKET�ɹ�" << std::endl;

	// 2 ���ӷ����� connect
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int nAddrLen = sizeof(sockaddr_in);
	int ret = connect(SockCli, (sockaddr*)&addrSer, nAddrLen);
	if (SOCKET_ERROR == ret)
		std::cout << "���ӷ�����ʧ��" << std::endl;
	else
		std::cout << "���ӷ������ɹ�" << std::endl;

	// 3 ���շ�������Ϣ recv
	char RecvBuf[256] = {};
	while (1) {
		int nLen = recv(SockCli, RecvBuf, 256, 0);
		if (nLen > 0)
			std::cout << "���յ����ݣ�" << RecvBuf << std::endl;
	}

	// 4 �ر����ӽ�
	closesocket(SockCli);

	WSACleanup();
	return 0;
}