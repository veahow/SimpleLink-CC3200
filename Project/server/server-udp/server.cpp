// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

const int BUF_SIZE = 1400;    // 缓冲区
const int PORT_NUM = 8000;    // 端口号

SOCKET sockServ;    // 服务器端sockServ
SOCKADDR_IN addrServ, addrCli;    // 服务器端地址addrServ 客户端地址addrCli
int addrCliLen = sizeof(addrCli);

int naddr = sizeof(SOCKADDR_IN);

char recvBuf[BUF_SIZE];    // 接收字符数组缓冲区
char sendBuf[BUF_SIZE];    // 发送字符数组缓冲区


DWORD WINAPI CreateClientThread(LPVOID lpParam);

int main()
{
	// 首先要加载socket库
	WSADATA wsaData;
	WORD wVersionRequested;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err) {
		std::cout << "载入socket库失败！按任意键退出程序" << std::endl;

		getchar();
		return 0;
	}

	// 创建socket
	sockServ = socket(AF_INET, SOCK_DGRAM, 0);

	// 初始化服务器端的端点地址（IP地址+Port端口号）
	addrServ.sin_addr.S_un.S_addr = inet_addr("172.26.246.253");    // 实验室网络 把本地电脑作为服务器ip
	//addrServ.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(PORT_NUM);    // 端口号8000

	// 绑定socket
	bind(sockServ, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
	// 循环监听 可以进行多线程编程改进
	for (;;) {
		if (recvfrom(sockServ, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&addrCli, &addrCliLen) == SOCKET_ERROR) {
			std::cout << "Recv Error" << std::endl;
			break;
		}

		std::cout << recvBuf << std::endl;

	}

	closesocket(sockServ);    // 关闭服务器端套接字
	WSACleanup();

	return 0;
}

DWORD WINAPI CreateClientThread(LPVOID lpParam)
{
	SOCKET sockCli = (SOCKET)lpParam;
	

	return 0;
}

