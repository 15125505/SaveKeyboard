// SaveKeyboardDlg.h : 头文件
//

#pragma once
#include <deque>
#include "UdpX.h"
#include <afxmt.h>


// CSaveKeyboardDlg 对话框
class CSaveKeyboardDlg : public CDialog
{
// 构造
public:
	CSaveKeyboardDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SAVEKEYBOARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

	// 键盘消息响应事件
	static BOOL WINAPI OnKeyboard(LPVOID lpContext, WPARAM wParam, LPARAM lParam);

	// 显示消息提示
	BOOL ShowToolTip(LPCTSTR szMsg,LPCTSTR szTitle,DWORD dwInfoFlags,UINT uTimeout);

	// 关闭消息提示
	BOOL CloseToolTip();

	//系统托盘的消息回调函数
	LRESULT OnShellNotify(WPARAM wParam, LPARAM lParam);

	//添加日志
	void AddLog();

	//日志信息回调函数
	static void LogCallback(
		void * lpContext, 
		const void *pData, 
		int nLen, 
		const char * lpszIP
		);

	// 设置开机自动启动
	BOOL SetAutoStart(BOOL bStart);

	// 键盘按键消息节点
	struct Node 
	{
		WPARAM wKey;
		DWORD dwTick;
	};
	std::deque<Node> m_deqKeys;

	// 获取精确的Tick
	DWORD Tick()
	{
		LARGE_INTEGER count, freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&count);
		return (DWORD)(count.QuadPart * 1000 / freq.QuadPart);
	}

	// 日志模块
	CRichEditCtrl m_ctrlLog;

	// 待处理日志信息列表
	struct LogNode 
	{
		int nType1;
		int nType2;
		CString strLog;
	};
	std::vector<LogNode> m_vecLog;
	CCriticalSection m_csLog;

	// 日志接收模块
	Tool::CUdpX m_LogRecv;


	// 系统托盘数据
	NOTIFYICONDATA	m_nid;	

	// 当前设置的时间间隔
	volatile int m_nTime;

};
