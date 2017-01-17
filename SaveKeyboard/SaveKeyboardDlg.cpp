// SaveKeyboardDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SaveKeyboard.h"
#include "SaveKeyboardDlg.h"
#include "KeyHook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSaveKeyboardDlg �Ի���




CSaveKeyboardDlg::CSaveKeyboardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveKeyboardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSaveKeyboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSaveKeyboardDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSaveKeyboardDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSaveKeyboardDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSaveKeyboardDlg ��Ϣ�������

BOOL CSaveKeyboardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	StartHook(CSaveKeyboardDlg::OnKeyboard, this);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSaveKeyboardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSaveKeyboardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSaveKeyboardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSaveKeyboardDlg::OnBnClickedOk()
{
}



void CSaveKeyboardDlg::OnBnClickedCancel()
{
	StopHook();

	OnCancel();
}

BOOL WINAPI CSaveKeyboardDlg::OnKeyboard(LPVOID lpContext, WPARAM wParam, LPARAM lParam)
{
	if ((lParam >> 30) != 0)
	{
		return TRUE;
	}

	CSaveKeyboardDlg *pThis = (CSaveKeyboardDlg *)lpContext;

	if (pThis->m_deqKeys.size() > 20)
	{
		pThis->m_deqKeys.pop_front();
	}
	DWORD dwTick = pThis->Tick();
	for (size_t i=0; i<pThis->m_deqKeys.size(); i++)
	{
		const Node & n = pThis->m_deqKeys[pThis->m_deqKeys.size() - i - 1];
		if (wParam == n.wKey && dwTick - n.dwTick < 120)
		{
			TRACE("�����������¼���%c %08X %d\n", wParam, lParam, dwTick - n.dwTick);
			return FALSE;
		}
	}

	Node node;
	node.wKey = wParam;
	node.dwTick = dwTick;
	pThis->m_deqKeys.push_back(node);

	return TRUE;
}
