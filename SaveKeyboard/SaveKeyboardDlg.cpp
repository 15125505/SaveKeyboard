// SaveKeyboardDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SaveKeyboard.h"
#include "SaveKeyboardDlg.h"
#include "../KeyHook/KeyHook.h"
#include "ZLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define Log1 LogN(1, 1)
#define Log2 LogN(2, 1)
#define Log3 LogN(3, 1)

#define WM_MSG_NOTIFY WM_USER+1537


#pragma data_seg("SaveKeyboardApp") 
HWND g_hWnd = NULL;
#pragma data_seg() 
#pragma comment(linker,"/SECTION:SaveKeyboardApp,RWS")

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
	m_nTime = 100;
}

void CSaveKeyboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG, m_ctrlLog);
}

BEGIN_MESSAGE_MAP(CSaveKeyboardDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSaveKeyboardDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSaveKeyboardDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_MSG_NOTIFY, OnShellNotify)
	ON_BN_CLICKED(IDC_AUTO_START, &CSaveKeyboardDlg::OnBnClickedAutoStart)
	ON_BN_CLICKED(IDC_SHOW_TIP, &CSaveKeyboardDlg::OnBnClickedShowTip)
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

	//��ʼ����־����ģ��
	int nPort = 2017;
	while (!m_LogRecv.StartReceive(++nPort, LogCallback, this, "127.0.0.1", "", false) && nPort < 2112){}
	CZLog::SetPort(nPort);

	// ��������
	m_nTime = AfxGetApp()->GetProfileInt("Config", "TimeGate", 40);
	SetDlgItemInt(IDC_TIME, m_nTime);
	Log1("��������Ϊ��%d����", m_nTime);

	((CButton *)GetDlgItem(IDC_AUTO_START))->SetCheck(IsAutoStart());
	((CButton *)GetDlgItem(IDC_SHOW_TIP))->SetCheck(AfxGetApp()->GetProfileInt("Config", "ShowTip", 0));


	// ������ʱ��������ͬ����־��
	SetTimer(1, 100, NULL);

	// �������̹���
	StartHook(CSaveKeyboardDlg::OnKeyboard, this);

	// ��ʼ��ϵͳ��������
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nid.hWnd = GetSafeHwnd();
	_stprintf_s(m_nid.szTip, _T("%s"), _T("������ļ���"));
	m_nid.uCallbackMessage = WM_MSG_NOTIFY;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE |NIF_TIP;
	m_nid.uID = IDR_MAINFRAME;

	// ��ʾ����ͼ��
	Shell_NotifyIcon(NIM_ADD, &m_nid);

	// ���浱ǰʵ�����
	if (NULL == g_hWnd)
	{
		g_hWnd = m_hWnd;
	}
	else
	{
		return TRUE;
	}


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

	//����Ի�����С��������������Ƶ���ڣ�������ʾ������Ƶ����
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		ShowToolTip("������ں�̨ĬĬ���м�أ���ֹ���ļ��̷���������", "���������ļ���", 0, 300);
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
	int nTime = GetDlgItemInt(IDC_TIME);
	if (nTime < 1 || nTime > 10000)
	{
		AfxMessageBox("������Ϸ���ʱ����(1~10000)��", MB_ICONINFORMATION);
		GetDlgItem(IDC_TIME)->SetFocus();
		return;
	}
	m_nTime = nTime;
	AfxGetApp()->WriteProfileInt("Config", "TimeGate", m_nTime);
	Log1("�������޸���Ϊ��%d����", m_nTime);
}


void CSaveKeyboardDlg::OnBnClickedCancel()
{
	// ���ڿɼ���ʱ�򵯳��Ի���ѯ��
	if (IsWindowVisible())
	{
		if (AfxMessageBox("ȷ��Ҫ�˳���", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1) == IDCANCEL)
		{
			return;
		}
	}

	// ֹͣ���ӳ���
	StopHook();

	//�˳�ǰɾ��ϵͳ����ͼ��
	Shell_NotifyIcon(NIM_DELETE, &m_nid);

	OnCancel();
}

// ���̰��µĻص�����
BOOL WINAPI CSaveKeyboardDlg::OnKeyboard(LPVOID lpContext, WPARAM wParam, LPARAM lParam)
{
	CSaveKeyboardDlg *pThis = (CSaveKeyboardDlg *)lpContext;
	KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;  
	TRACE("�����¼� -- %08X %s flag=%d scanCode=%d time=%d extra=%d\n", wParam, pThis->VK2Name(pkbhs->vkCode), pkbhs->flags, pkbhs->scanCode, pkbhs->time, pkbhs->dwExtraInfo); // ������Ϣ

	// ��¼ÿ�ΰ��������ʱ�䣬������ΰ��µ�ʱ�䣬���ϴε����ʱ�����̫������ô���Ա��ΰ���

	// �����������Ϣ��ֻ���м�¼�������д���
	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
	{
		if (pThis->m_deqKeys.size() > 20)
		{
			pThis->m_deqKeys.pop_front();
		}
		Node node;
		node.wKey = pkbhs->vkCode;
		node.dwTick = pkbhs->time;
		pThis->m_deqKeys.push_back(node);
		return TRUE;
	}

	// �������е����ݣ����ĳ�������ϴΰ��µ�ʱ������ƶ���ʱ�䷶Χ֮�ڣ���ô��Ϊ����
	DWORD dwCode = pkbhs->vkCode;
	DWORD dwTick = pkbhs->time;
	for (size_t i=0; i<pThis->m_deqKeys.size(); i++)
	{
		const Node & n = pThis->m_deqKeys[pThis->m_deqKeys.size() - i - 1];
		if (dwCode == n.wKey)
		{
			if (dwTick - n.dwTick < (DWORD)pThis->m_nTime)
			{
				Log2("<%s>������������������ʱ��Ϊ��%d����", pThis->VK2Name(dwCode), dwTick - n.dwTick);
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CSaveKeyboardDlg::AddLog()
{
	//��ȡ��ǰ����������
	static std::vector<LogNode> vec;
	m_csLog.Lock();
	vec.swap(m_vecLog);
	m_csLog.Unlock();
	if (vec.size() == 0)
	{
		return;
	}

	//����ѡ��λ��
	CHARRANGE cr;
	m_ctrlLog.GetSel(cr);

	//���ô�����ı���ʽ
	CHARFORMAT cf = {0};
	cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
	cf.yHeight	= 180;
	sprintf_s(cf.szFaceName, "����");
	m_ctrlLog.SetSel(-1, -1);
	COLORREF aColor[] = 
	{
		RGB(0, 0, 255),
		RGB(0, 150, 0),
		RGB(112, 48, 160),
	};

	//��һ������־��Ϣ
	for (size_t i=0; i<vec.size(); i++)
	{
		LogNode &node = vec[i];

		//ɾ���������
		while (m_ctrlLog.GetLineCount() > 5000)
		{
			int nPos = m_ctrlLog.LineLength(0);
			m_ctrlLog.SetSel(0, nPos + 1);
			m_ctrlLog.ReplaceSel("");
			cr.cpMax -= nPos + 1;
			cr.cpMin -= nPos + 1;
			if (cr.cpMax < 0)
			{
				cr.cpMax = 0;
			}
			if (cr.cpMin < 0)
			{
				cr.cpMin = 0;
			}
			m_ctrlLog.SetSel(-1, -1);
		}

		//����ʱ����Ϣ
		cf.crTextColor = aColor[0];
		m_ctrlLog.SetSelectionCharFormat(cf);
		m_ctrlLog.ReplaceSel(CTime::GetCurrentTime().Format("%m-%d %H:%M:%S  "));

		//������־����
		cf.crTextColor = aColor[node.nType1 % 3];
		m_ctrlLog.SetSelectionCharFormat(cf);
		m_ctrlLog.ReplaceSel(node.strLog);
		m_ctrlLog.ReplaceSel("\r\n");

		//�����ǰ���ڱ������ˣ�����Ҫ��ʾ��Ϣ��ʾ
		if (!IsWindowVisible() && node.strLog.Find("������������") >= 0)
		{
			ShowToolTip(node.strLog, "������ʾ", 0, 100);
		}
	}
	vec.clear();

	//�趨���λ��
	if (cr.cpMin != cr.cpMax)
	{
		//�ָ��û�ѡ��λ��
		m_ctrlLog.SetSel(cr);
	}
	else
	{
		//����������λ��
		m_ctrlLog.SendMessage(WM_VSCROLL,SB_BOTTOM, 0);
	}

}

//��־��Ϣ�ص�����
void CSaveKeyboardDlg::LogCallback(void * lpContext, const void *pData, int nLen, const char * lpszIP)
{
	CSaveKeyboardDlg *pThis = (CSaveKeyboardDlg *)lpContext;
	LogNode node;
	char *pTmp = (char *)pData;

	//��ȡ����1
	memcpy(&node.nType1, pTmp, 4);
	pTmp += 4;
	nLen -= 4;

	//��ȡ����2
	memcpy(&node.nType2, pTmp, 4);
	pTmp += 4;
	nLen -= 4;

	//��ȡ�ַ�������
	WORD wLen = 0;
	memcpy(&wLen, pTmp, 2);
	pTmp += 2;
	nLen -= 2;

	//ʣ�µ�����Ӧ��ȫ�����ַ�������
	if (wLen != (WORD)nLen)
	{
		ASSERT(0);
		return;
	}

	//��ȡ�ַ���
	node.strLog.Append(pTmp, nLen);

	TRACE("<%d, %d>%s\n", node.nType1, node.nType2, node.strLog);

	//���ַ��������б�
	pThis->m_csLog.Lock();
	if ((int)pThis->m_vecLog.size() >= 500)
	{
		pThis->m_vecLog.clear();
	}
	pThis->m_vecLog.push_back(node);
	pThis->m_csLog.Unlock();

}

// ��ʱ������������ͬ����־��Ϣ
void CSaveKeyboardDlg::OnTimer(UINT_PTR nIDEvent)
{
	static BOOL bFirst = TRUE;
	if (bFirst)
	{
		bFirst = FALSE;
		ShowWindow(SW_SHOWMINIMIZED);
		ShowWindow(SW_HIDE);
	}
	AddLog();
	CDialog::OnTimer(nIDEvent);
}

// ϵͳ���̵���Ϣ�ص�����
LRESULT CSaveKeyboardDlg::OnShellNotify(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDBLCLK || lParam == WM_LBUTTONDOWN) 
	{
		ShowWindow(SW_SHOW);
		ShowWindow(SW_RESTORE);
		SetForegroundWindow();
	}
	else if(lParam == WM_RBUTTONDOWN)
	{
		CPoint point;
		GetCursorPos(&point);
		PostMessage(WM_CONTEXTMENU, 0, MAKELPARAM(point.x, point.y));	
	}
	return 0;
}

// ��ʾ������ʾ
BOOL CSaveKeyboardDlg::ShowToolTip(LPCTSTR szMsg,LPCTSTR szTitle,DWORD dwInfoFlags,UINT uTimeout)
{
	if (!GetDlgItemInt(IDC_SHOW_TIP))
	{
		return TRUE;
	}
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.uFlags = NIF_INFO;
	m_nid.uVersion = NOTIFYICON_VERSION;
	m_nid.uTimeout = uTimeout;
	m_nid.dwInfoFlags = dwInfoFlags;
	strcpy_s( m_nid.szInfoTitle, szTitle );//���ݱ���
	strcpy_s( m_nid.szInfo,      szMsg     );//��������
	return Shell_NotifyIcon( NIM_MODIFY, &m_nid);
}

// �ر�������ʾ
BOOL CSaveKeyboardDlg::CloseToolTip()
{
	m_nid.cbSize=sizeof(NOTIFYICONDATA);
	m_nid.uFlags = NIF_INFO;
	strcpy_s( m_nid.szInfo,      _T("")     );//�������� ����Ϊ�ռ���ȡ��������ʾ
	return Shell_NotifyIcon( NIM_MODIFY, &m_nid );
}


// ���ÿ����Զ�����
void CSaveKeyboardDlg::OnBnClickedAutoStart()
{
	CButton *pButton = (CButton *)GetDlgItem(IDC_AUTO_START);
	if (!SetAutoStart(pButton->GetCheck()))
	{
		pButton->SetCheck(!pButton->GetCheck());
		AfxMessageBox("������������Զ�����δ�ܳɹ������ڱ�֤����ǽ����Ա�����ĸ���Ϊ�������ص���������ԡ�", MB_ICONINFORMATION);
	}
}

void CSaveKeyboardDlg::OnBnClickedShowTip()
{
	int nCheck = ((CButton *)GetDlgItem(IDC_SHOW_TIP))->GetCheck();
	AfxGetApp()->WriteProfileInt("Config", "ShowTip", nCheck);
}


// �����Ƿ񿪻��Զ�����
BOOL CSaveKeyboardDlg::SetAutoStart(BOOL bStart)
{
	HKEY hKey;
	LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 
		0, 
		KEY_ALL_ACCESS, 
		&hKey);
	if (ERROR_SUCCESS != lRet) 
	{
		return FALSE;
	}
	if (bStart) 
	{
		CString str;
		GetModuleFileName(NULL, str.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
		str.ReleaseBuffer();
		lRet = RegSetValueEx(hKey, 
			_T("ZClock"), 
			NULL, 
			REG_SZ, 
			(LPBYTE)(LPCTSTR)str, 
			str.GetLength() * sizeof(TCHAR));
	}
	else
	{
		lRet = RegDeleteValue(hKey, _T("ZClock"));
	}
	RegCloseKey(hKey);
	return (ERROR_SUCCESS ==  lRet);

}

// ��鵱ǰ�Ƿ񿪻��Զ�����
BOOL CSaveKeyboardDlg::IsAutoStart()
{
	HKEY hKey;
	LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 
		0, 
		KEY_ALL_ACCESS, 
		&hKey);
	if (ERROR_SUCCESS != lRet) 
	{
		return FALSE;
	}
	CString str, strPath;
	DWORD dwType = REG_SZ, dwLength = _MAX_PATH * sizeof(TCHAR);
	lRet = RegQueryValueEx(hKey, 
		_T("ZClock"), 
		NULL, 
		&dwType, 
		(LPBYTE)str.GetBufferSetLength(_MAX_PATH), 
		&dwLength);
	RegCloseKey(hKey);
	if (lRet != ERROR_SUCCESS) 
	{
		return FALSE;
	}

	str.ReleaseBuffer();
	GetModuleFileName(NULL, strPath.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
	strPath.ReleaseBuffer();
	strPath.MakeLower();
	str.MakeLower();
	if (strPath != str) 
	{
		return FALSE;
	}
	return TRUE;
}

CString CSaveKeyboardDlg::VK2Name(WPARAM wp)
{
	char   str[100] = {0}; 
	::GetKeyNameText(::MapVirtualKey(wp, 0) << 16, str, 100); 
	return str;
}


