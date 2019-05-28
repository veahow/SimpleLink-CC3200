// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

const int BUF_SIZE = 1400;    // 缓冲区
const int PORT_NUM = 5001;    // 端口号
const int queuesize = 5;    // 最多连5个
int numLinked = 0;    // 连接数

SOCKET sockServ, mainSockCli;    // 服务器端sockServ 客户端sockCli
SOCKADDR_IN addrServ, addrCli;    // 服务器端地址addrServ 客户端地址addrCli
SOCKET sock[queuesize];    // 这里其实可以用队列 使用数组代替

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
	sockServ = socket(AF_INET, SOCK_STREAM, 0);

	// 初始化服务器端的端点地址（IP地址+Port端口号）
	addrServ.sin_addr.S_un.S_addr = inet_addr("192.168.43.188");    // 实验室网络 把本地电脑作为服务器ip
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(PORT_NUM);    // 端口号5001

	// 绑定socket
	bind(sockServ, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
	listen(sockServ, queuesize);
	// 循环监听 可以进行多线程编程改进
	for (;;) {
		// 监听连接请求
		//listen(sockServ, queuesize);

		// 接受连接请求
		mainSockCli = accept(sockServ, (SOCKADDR *)&addrCli, &naddr);    // 放在创建的客户端套接字中

		if (mainSockCli != INVALID_SOCKET) {
			//::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)mainSockCli, 0, nullptr);
			std::cout << "连接成功！" << std::endl;

			recv(mainSockCli, recvBuf, BUF_SIZE, 0);
			std::cout << recvBuf << std::endl;

			//send(mainSockCli, "ni hao a", BUF_SIZE, 0);
		}

	}

	closesocket(mainSockCli);    // 关闭客户端主套接字
	closesocket(sockServ);    // 关闭服务器端套接字
	WSACleanup();

	return 0;
}

DWORD WINAPI CreateClientThread(LPVOID lpParam)
{
	SOCKET sockCli = (SOCKET)lpParam;
	char ip[BUF_SIZE];
	char recvBuf[BUF_SIZE];    // 接收字符数组缓冲区
	char buf[BUF_SIZE];

	memset(ip, 0, sizeof(ip));
	memset(recvBuf, 0, sizeof(recvBuf));
	memset(buf, 0, sizeof(buf));

	recv(sockCli, ip, BUF_SIZE, 0);
	//ip[strlen(ip) - 1] = '\0';
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		FOREGROUND_BLUE |
		FOREGROUND_GREEN |
		FOREGROUND_INTENSITY);
	std::cout << ip << " 连接服务器成功" << std::endl;

	sock[numLinked++] = sockCli;

	//::CreateThread(nullptr, 0, TransmitInfo, (LPVOID)sockCli, 0, nullptr);
	for (;;) {
		memset(buf, 0, sizeof(buf));
		memset(recvBuf, 0, sizeof(recvBuf));

		if (recv(sockCli, recvBuf, BUF_SIZE, 0) <= 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::cout << ip << " 退出服务器" << std::endl;
			break;
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		std::cout << "[" << ip << "]:" << recvBuf << std::endl;
		sprintf_s(buf, "[%s]:%s\n", ip, recvBuf);
		for (int i = 0; i < numLinked; ++i) {
			//if (sock[i] != sockCli) send(sock[i], buf, BUF_SIZE, 0);
			send(sock[i], buf, BUF_SIZE, 0);
		}

	}

	closesocket(sockCli);

	return 0;
}

