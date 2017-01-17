// SaveKeyboardDlg.h : ͷ�ļ�
//

#pragma once
#include <deque>


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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	// ������Ϣ��Ӧ�¼�
	static BOOL WINAPI OnKeyboard(LPVOID lpContext, WPARAM wParam, LPARAM lParam);

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

};
