/*******************************************************************************
* ��Ȩ���� (C) 2009
* 
* �ļ����ƣ� UdpX.h
* �ļ���ʶ�� 
* ����ժҪ�� Udp�շ������ࡣ
* ����˵���� 
*		����˵�� :
*			������͵�Ŀ���ַ����һ����ͬ����ģ����Զ�ʹ���ϴε�socket���ͣ�
*			���Ҫ������ַ��Ϣֱ�ӷ������ݣ���ģ����Զ�ʹ���ϴε�socket���ͣ�
*		����˵�� :
* ��ǰ�汾�� V2.0
* ��    �ߣ� �ܷ�
* ������ڣ� 2009-03-31
* �޸�ʱ��	�޸���	�޸�����
--------------------------------------------------------------------------------
2009-03-31	�ܷ�	����SetSendIP���иĽ���ʹ���ܹ��ظ����ö�Ρ�	
2010-05-21	�ܷ�	�������շ���ʹ�÷�ʽ�������ΪLinux�µĿ��ð汾��
*******************************************************************************/
#ifndef _UDP_X_78953472894839217483218948329
#define _UDP_X_78953472894839217483218948329

#include <vector>
#include <string>

#if (defined _WIN32) || (defined _WINDOWS_)
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <pthread.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace Tool
{

typedef void (*DataReceiveCallbackType)(
										void * lpContext, 
										const void *pData, 
										int nLen, 
										const char * lpszIP
										);

class CUdpX
{
public:
	CUdpX(void);
	~CUdpX(void);

	//��������(�����ַδ�����仯���򲻻����´���socket)
	bool Send(const char * lpszIP, 
		int nPort, 
		const char * lpszLocalIP, 
		const void *pData = NULL, 
		int nLen = 0);

	//��������(���ݽ������͵��ϴ����õĵ�ַ)
	bool Send(const void *pData, int nLen);

	//��ʼ��������
	bool StartReceive(
		int nPort, 
		DataReceiveCallbackType pfnData, 
		void * lpContext, 
		const char * lpszLocalIP = "", 
		const char * lpszMultiIP = "",
		bool bReuse = true
		);

	//ֹͣ��������
	void StopReceive();

	//����ֹͣ���ձ��λ
	void SetStopFlag(){m_Recv.bStop = true;}

protected:

#if (defined _WIN32) || (defined _WINDOWS_)
	typedef SOCKET SOCKET_HANDLE;
	typedef const char * VAL_TYPE;
	typedef HANDLE	THREAD_HANDLE;
	typedef UINT THREAD_RET;
#else
	typedef int SOCKET_HANDLE;
	typedef const void * VAL_TYPE;
	typedef pthread_t	THREAD_HANDLE;
	typedef void * THREAD_RET;
#endif

#define INVALID_TH_HANDLE (THREAD_HANDLE)(-1)

	//���ݽ��սڵ�
	struct  RecvInfo
	{
		//�߳̾��
		THREAD_HANDLE hThread;

		//�¼�ֹͣ���
		bool bStop;

		//���ݷ����ص�����ָ��
		DataReceiveCallbackType pfnData;

		//���ݷ�����������
		void * lpContext;

		//����IP��ַ
		std::string strLocalIP;

		//�鲥��ϢIP��ַ
		std::string strMultiIP;

		//�˿ڵ�ַ
		int nPort;

		//Socket���
		SOCKET_HANDLE s;

		//clear
		void Clear()
		{
			hThread = INVALID_TH_HANDLE;
			bStop = false;
			pfnData = 0;
			lpContext = 0;
			nPort = 0;
			s = 0;
			strLocalIP = strMultiIP = "";
		}

	}m_Recv;

	struct SendInfo 
	{
		//Ŀ��IP��ַ
		std::string strDstIP;

		//����������ַ
		std::string strLocalIP;

		//Ŀ��˿�
		int nPort;

		//���ڷ��͵�socket���
		SOCKET_HANDLE s;

		//�û����õ�Ŀ�귢�͵�ַ
		sockaddr_in addr;

		SendInfo() : s(0) , nPort(0){}

	}m_Send;

	//�̺߳���
	static THREAD_RET TH_Receive(void * lp);

	//IP��ַת������
	static unsigned long IP2Addr(const char * lpszIP);

	//�ر�socket
	static void CloseSocket(SOCKET_HANDLE &s);

};
}
#endif
