/*******************************************************************************
* 版权所有 (C) 2009
* 
* 文件名称： UdpX.h
* 文件标识： 
* 内容摘要： Udp收发辅助类。
* 其它说明： 
*		发送说明 :
*			如果发送的目标地址和上一次相同，则本模块会自动使用上次的socket发送；
*			如果要不带地址信息直接发送数据，则本模块会自动使用上次的socket发送；
*		接收说明 :
* 当前版本： V2.0
* 作    者： 周锋
* 完成日期： 2009-03-31
* 修改时间	修改人	修改内容
--------------------------------------------------------------------------------
2009-03-31	周锋	对于SetSendIP进行改进，使其能够重复设置多次。	
2010-05-21	周锋	简化数据收发的使用方式，改造成为Linux下的可用版本。
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

	//发送数据(如果地址未发生变化，则不会重新创建socket)
	bool Send(const char * lpszIP, 
		int nPort, 
		const char * lpszLocalIP, 
		const void *pData = NULL, 
		int nLen = 0);

	//发送数据(数据将被发送到上次设置的地址)
	bool Send(const void *pData, int nLen);

	//开始接受数据
	bool StartReceive(
		int nPort, 
		DataReceiveCallbackType pfnData, 
		void * lpContext, 
		const char * lpszLocalIP = "", 
		const char * lpszMultiIP = "",
		bool bReuse = true
		);

	//停止接收数据
	void StopReceive();

	//设置停止接收标记位
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

	//数据接收节点
	struct  RecvInfo
	{
		//线程句柄
		THREAD_HANDLE hThread;

		//事件停止标记
		bool bStop;

		//数据反馈回调函数指针
		DataReceiveCallbackType pfnData;

		//数据反馈环境变量
		void * lpContext;

		//本机IP地址
		std::string strLocalIP;

		//组播信息IP地址
		std::string strMultiIP;

		//端口地址
		int nPort;

		//Socket句柄
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
		//目标IP地址
		std::string strDstIP;

		//本地网卡地址
		std::string strLocalIP;

		//目标端口
		int nPort;

		//用于发送的socket句柄
		SOCKET_HANDLE s;

		//用户设置的目标发送地址
		sockaddr_in addr;

		SendInfo() : s(0) , nPort(0){}

	}m_Send;

	//线程函数
	static THREAD_RET TH_Receive(void * lp);

	//IP地址转换函数
	static unsigned long IP2Addr(const char * lpszIP);

	//关闭socket
	static void CloseSocket(SOCKET_HANDLE &s);

};
}
#endif
