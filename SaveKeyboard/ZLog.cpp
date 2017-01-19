#include "StdAfx.h"
#include "ZLog.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4267)
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <stdarg.h>
#include "ZLog.h"

int CZLog::m_sPort = 2013;

CZLog::CZLog(int nType1, int nType2) : m_nType1(nType1), m_nType2(nType2)
{
}

CZLog::~CZLog(void)
{
}

void CZLog::operator()(const char *pszFmt, ...) const
{
	char pszMsg[1024];
	va_list ptr; 
	va_start(ptr, pszFmt);
	vsprintf(pszMsg, pszFmt, ptr); 
	va_end(ptr);

	SendMsg(pszMsg);	
}

void CZLog::SendMsg(const char * lpszMsg) const
{
	//待发送缓冲区
	char buff[2000];
	int nLen = 0;

	//加入类型1
	char *pData = buff;
	memcpy(pData, &m_nType1, 4);
	pData += 4;
	nLen += 4;

	//加入类型2
	memcpy(pData, &m_nType2, 4);
	pData += 4;
	nLen += 4;

	//加入字符串长度
	WORD wLen = (WORD)strlen(lpszMsg);
	memcpy(pData, &wLen, 2);
	pData += 2;
	nLen += 2;

	//加入字符串内容
	memcpy(pData, lpszMsg, wLen);
	pData += wLen;
	nLen += wLen;

	//发送数据到网络
	WSADATA wsaData;
	static bool bInited = false;
	if (!bInited)
	{
		bInited = true;
		WSAStartup(MAKEWORD(2,2), &wsaData);
	}
	SOCKET SendSocket;
	struct sockaddr_in RecvAddr;
	if (-1 == (SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
	{
		return;
	}
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(m_sPort);
	RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int nSend = sendto(
		SendSocket, 
		buff, 
		nLen, 
		0,
		(sockaddr*) &RecvAddr, 
		(int)sizeof(RecvAddr));
	closesocket(SendSocket);
}