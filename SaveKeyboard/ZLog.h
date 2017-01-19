#pragma once

class CZLog
{
public:
	//���캯��
	CZLog(int nType1, int nType2);

	//��������
	~CZLog(void);

	//������"()"������
	void operator()(const char *pszFmt, ...) const;

	//������־����
	static void SetPort(int nPort) {m_sPort = nPort;}

private:
	static int m_sPort;				//��־�˿�
	const int m_nType1;				//��־����1
	const int m_nType2;				//��־����2

	//�����־����
	void SendMsg(const char * lpszMsg) const;
};


//���������������־��һ���
#define LogN(m, n)		CZLog(m, n)
#define LogMsg			LogN(0, 0)

