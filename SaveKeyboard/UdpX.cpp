#include "stdafx.h"
#include "UdpX.h"

#ifndef HIDE_UDP_X_LOG
#ifdef LogN
#define Log LogN(4000)
#elif (defined TRACE)
#define Log TRACE
#else
#define Log printf
#endif
#else
#define Log
#endif

namespace Tool
{

CUdpX::CUdpX(void)
{
	Log("Func -- CUdpX::CUdpX(void)");

#if (defined _WIN32) || (defined _WINDOWS_)
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
	m_Recv.Clear();
}

CUdpX::~CUdpX(void)
{
	Log("Func -- CUdpX::~CUdpX(void)");

	//停止接收数据
	StopReceive();

	//关闭发送端
	CloseSocket(m_Send.s);

#if (defined _WIN32) || (defined _WINDOWS_)
	int nRet = WSACleanup();
#endif
}

//发送数据
bool CUdpX::Send(const char * lpszIP, int nPort, const char * lpszLocalIP, const void *pData, int nLen)
{
	//如果IP地址发生变化，需要关闭以前的socket
	if (m_Send.strDstIP != lpszIP
		|| m_Send.nPort != nPort
		|| m_Send.strLocalIP != lpszLocalIP)
	{
		//记录用户配置
		m_Send.strDstIP = lpszIP;
		m_Send.strLocalIP = lpszLocalIP;
		m_Send.nPort = nPort;

		//设置输出信息
		m_Send.addr.sin_family = AF_INET;
		m_Send.addr.sin_port = htons(nPort);
		m_Send.addr.sin_addr.s_addr = IP2Addr(lpszIP);

		//关闭当前socket
		CloseSocket(m_Send.s);
	}

	//如果需要，则创建socket
	if (0 == m_Send.s)
	{
		m_Send.s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (0 == m_Send.s)
		{
			Log("创建UDP Socket句柄失败！");
			return false;
		}

		//设置端口复用
		int bSockReuse = 1;
		if ( 0 != setsockopt(m_Send.s, 
			SOL_SOCKET,
			SO_REUSEADDR,
			(VAL_TYPE)&bSockReuse, sizeof(bSockReuse)))
		{
			Log("设置端口复用失败");
		}

		//邦定socket
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = 0;
		addr.sin_addr.s_addr = IP2Addr(lpszLocalIP);
		if(0 != bind(m_Send.s, (sockaddr *) &addr, sizeof(addr)))
		{
			Log("端口地址绑定失败<%s:%d>！", lpszLocalIP, nPort);
			CloseSocket(m_Send.s);
			return false;
		}
	}

	//发送数据
	return Send(pData, nLen);
}


//发送数据(数据将被发送到预先设置的目标IP地址)
bool CUdpX::Send(const void *pData, int nLen)
{
	if (0 == m_Send.s)
	{
		return false;
	}
	if (0 == pData || nLen <= 0)
	{
		return true;
	}

	int nSended = sendto(m_Send.s, 
		(const char *)pData, 
		nLen, 
		0, 
		(sockaddr*) &m_Send.addr, 
		(int)sizeof(m_Send.addr));

	return (nSended == nLen);

}

//开始接收数据
bool CUdpX::StartReceive(
				int nPort, 
				DataReceiveCallbackType pfnData, 
				void * lpContext, 
				const char * lpszLocalIP, 
				const char * lpszMultiIP,
				bool bReuse
				)
{
	Log("Func -- bool CUdpX::StartReceive(%d, %p, %s, %s)", nPort, pfnData, lpszLocalIP, lpszMultiIP);

	if (INVALID_TH_HANDLE != m_Recv.hThread)
	{
		Log("不能重复启动数据接收！");
		return false;
	}

	//创建socket
	SOCKET_HANDLE s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


	//设置端口复用
	if (bReuse)
	{
		int bSockReuse = 1;
		int iRet = setsockopt(s, 
			SOL_SOCKET,
			SO_REUSEADDR,
			(VAL_TYPE)&bSockReuse, sizeof(bSockReuse));
		if (0 != iRet)
		{
			Log("端口复用失败! -- %d", iRet);
		}
	}

	//邦定socket
	sockaddr_in addrRecv;
	addrRecv.sin_family = AF_INET;
	addrRecv.sin_port = htons(nPort);
	addrRecv.sin_addr.s_addr =  IP2Addr(lpszLocalIP);

#if !(defined _WIN32) && !(defined _WINDOWS_)
	if (std::string(lpszMultiIP) != "")
	{
		addrRecv.sin_addr.s_addr =  IP2Addr(lpszMultiIP);
	}
#endif

	if(0 != bind(s, (sockaddr *) &addrRecv, sizeof(addrRecv)))
	{
		Log("端口地址绑定失败<%s:%d>！", lpszLocalIP, nPort);
		CloseSocket(s);
		return false;
	}

	//设置Socket缓冲(一般默认是8192)
	int nRcvBuffSize = 8192 * 100;
	if(0 != setsockopt(s, SOL_SOCKET, SO_RCVBUF, (VAL_TYPE)&nRcvBuffSize, sizeof(nRcvBuffSize)))
	{
		Log("设置端口缓存大小失败<%s:%d>！", lpszLocalIP, nPort);
	}

	//加入组播组
	struct ip_mreq mreq = {0};
	if (std::string(lpszMultiIP) != "")
	{
		mreq.imr_multiaddr.s_addr = IP2Addr (lpszMultiIP);
		mreq.imr_interface.s_addr = IP2Addr (lpszLocalIP);
		if (0 != setsockopt (s, 
			IPPROTO_IP, 
			IP_ADD_MEMBERSHIP, 
			(VAL_TYPE)&mreq, 
			sizeof (mreq)))
		{
			Log("加入组播组<%s>失败<%s:%d>！", lpszMultiIP, lpszLocalIP, nPort);
			CloseSocket(s);
			return false;
		}
	}

	m_Recv.s = s;
	m_Recv.nPort = nPort;
	m_Recv.pfnData = pfnData;
	m_Recv.lpContext = lpContext;
	m_Recv.strLocalIP = lpszLocalIP;
	m_Recv.strMultiIP = lpszMultiIP;

	//创建监听线程
	m_Recv.bStop = false;
#if (defined _WIN32) || (defined _WINDOWS_)
	CWinThread *pThread = AfxBeginThread(TH_Receive, this, THREAD_PRIORITY_TIME_CRITICAL);
	if (0 == pThread)
	{
		Log("创建监听线程失败！");
		m_Recv.Clear();
		return false;
	}
	m_Recv.hThread = pThread->m_hThread;
#else
	pthread_attr_t attr;
	sched_param param;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	param.sched_priority = sched_get_priority_max(SCHED_RR);
	pthread_attr_setschedparam(&attr, &param); 

	int nRet = pthread_create(&m_Recv.hThread, &attr, TH_Receive, this);
	if (0 != nRet)
	{
		Log("创建监听线程失败！");
		m_Recv.Clear();
		return false;
	}
#endif
	return true;
}


//停止指定端口的监听
void CUdpX::StopReceive()
{
	Log("Func -- void CUdpX::StopReceive()");

	if (INVALID_TH_HANDLE != m_Recv.hThread)
	{
		m_Recv.bStop = true;
#if (defined _WIN32) || (defined _WINDOWS_)
		WaitForSingleObject(m_Recv.hThread, INFINITE);
#else
		pthread_join(m_Recv.hThread, 0); 
#endif
	}

	m_Recv.Clear();
}

CUdpX::THREAD_RET CUdpX::TH_Receive(void * lp)
{
	Log("Func -- UINT CUdpX::TH_Receive(%p)", lp);

	CUdpX *pThis = (CUdpX *)lp;

	//获取网络上的数据
	char RecvBuf[2048] = {0};
	while (!pThis->m_Recv.bStop)
	{
		timeval timeout = {1, 0};

		//检查是否有数据到达
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(pThis->m_Recv.s, &fds);
		if(-1 == select((int)pThis->m_Recv.s + 1, &fds, 0, 0, &timeout) )
		{
			Log("select failed!");
			break;
		}
		if (!FD_ISSET(pThis->m_Recv.s, &fds))
		{
			continue;
		}

		//有数据到达
		sockaddr_in addrSender;
		socklen_t SenderAddrSize = sizeof(addrSender);
		int nCount = recvfrom(pThis->m_Recv.s, 
			RecvBuf, 
			2048, 
			0, 
			(sockaddr *)&addrSender, 
			&SenderAddrSize);

		//回调通知
		if (0 != pThis->m_Recv.pfnData)
		{
			pThis->m_Recv.pfnData(pThis->m_Recv.lpContext, RecvBuf, nCount, inet_ntoa(addrSender.sin_addr));
		}
	}

	//退出组播组
	struct ip_mreq mreq;
	if (pThis->m_Recv.strMultiIP != "")
	{
		mreq.imr_multiaddr.s_addr = IP2Addr(pThis->m_Recv.strMultiIP.c_str());
		mreq.imr_interface.s_addr = IP2Addr(pThis->m_Recv.strLocalIP.c_str());
		if (0 != setsockopt (pThis->m_Recv.s, 
			IPPROTO_IP, 
			IP_DROP_MEMBERSHIP, 
			(VAL_TYPE)&mreq, 
			sizeof (mreq)))
		{
			Log("退出组播组<%s>失败<%s:%d>！", pThis->m_Recv.strMultiIP.c_str(), 
				pThis->m_Recv.strLocalIP.c_str(), pThis->m_Recv.nPort);
		}
	}

	CloseSocket(pThis->m_Recv.s);

	Log("监听线程<端口 %d><本地 %s><组播 %s>退出。", 
		pThis->m_Recv.nPort, pThis->m_Recv.strLocalIP.c_str(), pThis->m_Recv.strMultiIP.c_str());

	return 0;
}

//IP地址转换函数
unsigned long CUdpX::IP2Addr(const char * lpszIP)
{
	if (0 == lpszIP || std::string(lpszIP) == "")
	{
		return htonl(INADDR_ANY);
	}
	return inet_addr(lpszIP);
}

//关闭socket
void CUdpX::CloseSocket(SOCKET_HANDLE &s)
{
	if (0 == s)
	{
		return;
	}
#if (defined _WIN32) || (defined _WINDOWS_)
	closesocket(s);
#else
	close(s);
#endif
	s = 0;
}


}

