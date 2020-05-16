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
	SOCKET SockSer = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == SockSer)
		std::cout << "����SOCKETʧ��" << std::endl;
	else
		std::cout << "����SOCKET�ɹ�" << std::endl;

	// 2 bind �����ڽ��տͻ������ӵ�����˿�
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = INADDR_ANY ;//inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(SockSer, (sockaddr*)&addrSer, sizeof(addrSer)))
		std::cout << "�󶨶˿�ʧ��" << std::endl;
	else
		std::cout << "�󶨶˿ڳɹ�" << std::endl;
	
	// 3 listen ��������˿�
	if(SOCKET_ERROR == listen(SockSer, 5))
		std::cout << "�����˿�ʧ��" << std::endl;
	else
		std::cout << "�����˿ڳɹ�" << std::endl;
	
	// 4 accpet �ȴ����տͻ�������
	sockaddr_in ClientAddr{};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET Client = INVALID_SOCKET;
	char SendBuf[] = "��ã�����Server~";

	// 5 send ��ͻ��˷���һ����Ϣ
	while (1) {
		Client = accept(SockSer, (sockaddr*)&ClientAddr, &nAddrLen);
		if (INVALID_SOCKET == Client)
			std::cout << "��Ч�Ŀͻ�������" << std::endl;
		std::cout << "�¿ͻ��˼��� IP = " << inet_ntoa(ClientAddr.sin_addr) << std::endl;
		
		send(Client, SendBuf, sizeof(SendBuf) + 1, 0);
	}

	// 6 �ر����ӽ�
	closesocket(SockSer);

	WSACleanup();
	return 0;
}