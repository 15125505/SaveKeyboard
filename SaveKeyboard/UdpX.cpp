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

	//ֹͣ��������
	StopReceive();

	//�رշ��Ͷ�
	CloseSocket(m_Send.s);

#if (defined _WIN32) || (defined _WINDOWS_)
	int nRet = WSACleanup();
#endif
}

//��������
bool CUdpX::Send(const char * lpszIP, int nPort, const char * lpszLocalIP, const void *pData, int nLen)
{
	//���IP��ַ�����仯����Ҫ�ر���ǰ��socket
	if (m_Send.strDstIP != lpszIP
		|| m_Send.nPort != nPort
		|| m_Send.strLocalIP != lpszLocalIP)
	{
		//��¼�û�����
		m_Send.strDstIP = lpszIP;
		m_Send.strLocalIP = lpszLocalIP;
		m_Send.nPort = nPort;

		//���������Ϣ
		m_Send.addr.sin_family = AF_INET;
		m_Send.addr.sin_port = htons(nPort);
		m_Send.addr.sin_addr.s_addr = IP2Addr(lpszIP);

		//�رյ�ǰsocket
		CloseSocket(m_Send.s);
	}

	//�����Ҫ���򴴽�socket
	if (0 == m_Send.s)
	{
		m_Send.s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (0 == m_Send.s)
		{
			Log("����UDP Socket���ʧ�ܣ�");
			return false;
		}

		//���ö˿ڸ���
		int bSockReuse = 1;
		if ( 0 != setsockopt(m_Send.s, 
			SOL_SOCKET,
			SO_REUSEADDR,
			(VAL_TYPE)&bSockReuse, sizeof(bSockReuse)))
		{
			Log("���ö˿ڸ���ʧ��");
		}

		//�socket
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = 0;
		addr.sin_addr.s_addr = IP2Addr(lpszLocalIP);
		if(0 != bind(m_Send.s, (sockaddr *) &addr, sizeof(addr)))
		{
			Log("�˿ڵ�ַ��ʧ��<%s:%d>��", lpszLocalIP, nPort);
			CloseSocket(m_Send.s);
			return false;
		}
	}

	//��������
	return Send(pData, nLen);
}


//��������(���ݽ������͵�Ԥ�����õ�Ŀ��IP��ַ)
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

//��ʼ��������
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
		Log("�����ظ��������ݽ��գ�");
		return false;
	}

	//����socket
	SOCKET_HANDLE s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


	//���ö˿ڸ���
	if (bReuse)
	{
		int bSockReuse = 1;
		int iRet = setsockopt(s, 
			SOL_SOCKET,
			SO_REUSEADDR,
			(VAL_TYPE)&bSockReuse, sizeof(bSockReuse));
		if (0 != iRet)
		{
			Log("�˿ڸ���ʧ��! -- %d", iRet);
		}
	}

	//�socket
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
		Log("�˿ڵ�ַ��ʧ��<%s:%d>��", lpszLocalIP, nPort);
		CloseSocket(s);
		return false;
	}

	//����Socket����(һ��Ĭ����8192)
	int nRcvBuffSize = 8192 * 100;
	if(0 != setsockopt(s, SOL_SOCKET, SO_RCVBUF, (VAL_TYPE)&nRcvBuffSize, sizeof(nRcvBuffSize)))
	{
		Log("���ö˿ڻ����Сʧ��<%s:%d>��", lpszLocalIP, nPort);
	}

	//�����鲥��
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
			Log("�����鲥��<%s>ʧ��<%s:%d>��", lpszMultiIP, lpszLocalIP, nPort);
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

	//���������߳�
	m_Recv.bStop = false;
#if (defined _WIN32) || (defined _WINDOWS_)
	CWinThread *pThread = AfxBeginThread(TH_Receive, this, THREAD_PRIORITY_TIME_CRITICAL);
	if (0 == pThread)
	{
		Log("���������߳�ʧ�ܣ�");
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
		Log("���������߳�ʧ�ܣ�");
		m_Recv.Clear();
		return false;
	}
#endif
	return true;
}


//ָֹͣ���˿ڵļ���
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

	//��ȡ�����ϵ�����
	char RecvBuf[2048] = {0};
	while (!pThis->m_Recv.bStop)
	{
		timeval timeout = {1, 0};

		//����Ƿ������ݵ���
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

		//�����ݵ���
		sockaddr_in addrSender;
		socklen_t SenderAddrSize = sizeof(addrSender);
		int nCount = recvfrom(pThis->m_Recv.s, 
			RecvBuf, 
			2048, 
			0, 
			(sockaddr *)&addrSender, 
			&SenderAddrSize);

		//�ص�֪ͨ
		if (0 != pThis->m_Recv.pfnData)
		{
			pThis->m_Recv.pfnData(pThis->m_Recv.lpContext, RecvBuf, nCount, inet_ntoa(addrSender.sin_addr));
		}
	}

	//�˳��鲥��
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
			Log("�˳��鲥��<%s>ʧ��<%s:%d>��", pThis->m_Recv.strMultiIP.c_str(), 
				pThis->m_Recv.strLocalIP.c_str(), pThis->m_Recv.nPort);
		}
	}

	CloseSocket(pThis->m_Recv.s);

	Log("�����߳�<�˿� %d><���� %s><�鲥 %s>�˳���", 
		pThis->m_Recv.nPort, pThis->m_Recv.strLocalIP.c_str(), pThis->m_Recv.strMultiIP.c_str());

	return 0;
}

//IP��ַת������
unsigned long CUdpX::IP2Addr(const char * lpszIP)
{
	if (0 == lpszIP || std::string(lpszIP) == "")
	{
		return htonl(INADDR_ANY);
	}
	return inet_addr(lpszIP);
}

//�ر�socket
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

