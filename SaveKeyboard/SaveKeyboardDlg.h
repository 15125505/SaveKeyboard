// SaveKeyboardDlg.h : 头文件
//

#pragma once
#include <deque>


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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	// 键盘消息响应事件
	static BOOL WINAPI OnKeyboard(LPVOID lpContext, WPARAM wParam, LPARAM lParam);

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

};
