// SaveKeyboardDlg.h : ͷ�ļ�
//

#pragma once
#include <deque>
#include "UdpX.h"
#include <afxmt.h>


// CSaveKeyboardDlg �Ի���
class CSaveKeyboardDlg : public CDialog
{
// ����
public:
	CSaveKeyboardDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SAVEKEYBOARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNcPaint();
	afx_msg void OnBnClickedAutoStart();

	// ������Ϣ��Ӧ�¼�
	static BOOL WINAPI OnKeyboard(LPVOID lpContext, WPARAM wParam, LPARAM lParam);

	// ��ʾ��Ϣ��ʾ
	BOOL ShowToolTip(LPCTSTR szMsg,LPCTSTR szTitle,DWORD dwInfoFlags,UINT uTimeout);

	// �ر���Ϣ��ʾ
	BOOL CloseToolTip();

	//ϵͳ���̵���Ϣ�ص�����
	LRESULT OnShellNotify(WPARAM wParam, LPARAM lParam);

	//�����־
	void AddLog();

	//��־��Ϣ�ص�����
	static void LogCallback(
		void * lpContext, 
		const void *pData, 
		int nLen, 
		const char * lpszIP
		);

	// ���ÿ����Զ�����
	BOOL SetAutoStart(BOOL bStart);

	// ���̰�����Ϣ�ڵ�
	struct Node 
	{
		WPARAM wKey;
		DWORD dwTick;
	};
	std::deque<Node> m_deqKeys;

	// ��ȡ��ȷ��Tick
	DWORD Tick()
	{
		LARGE_INTEGER count, freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&count);
		return (DWORD)(count.QuadPart * 1000 / freq.QuadPart);
	}

	// ��־ģ��
	CRichEditCtrl m_ctrlLog;

	// ��������־��Ϣ�б�
	struct LogNode 
	{
		int nType1;
		int nType2;
		CString strLog;
	};
	std::vector<LogNode> m_vecLog;
	CCriticalSection m_csLog;

	// ��־����ģ��
	Tool::CUdpX m_LogRecv;


	// ϵͳ��������
	NOTIFYICONDATA	m_nid;	

	// ��ǰ���õ�ʱ����
	volatile int m_nTime;

};
