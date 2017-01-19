#pragma once

class CZLog
{
public:
	//构造函数
	CZLog(int nType1, int nType2);

	//析构函数
	~CZLog(void);

	//操作符"()"的重载
	void operator()(const char *pszFmt, ...) const;

	//更新日志窗口
	static void SetPort(int nPort) {m_sPort = nPort;}

private:
	static int m_sPort;				//日志端口
	const int m_nType1;				//日志类型1
	const int m_nType2;				//日志类型2

	//输出日志内容
	void SendMsg(const char * lpszMsg) const;
};


//以下是用于输出日志的一组宏
#define LogN(m, n)		CZLog(m, n)
#define LogMsg			LogN(0, 0)

