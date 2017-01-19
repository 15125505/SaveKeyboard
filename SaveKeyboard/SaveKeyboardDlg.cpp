// SaveKeyboardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SaveKeyboard.h"
#include "SaveKeyboardDlg.h"
#include "KeyHook.h"
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CSaveKeyboardDlg 对话框




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
	ON_WM_NCPAINT()
	ON_BN_CLICKED(IDC_AUTO_START, &CSaveKeyboardDlg::OnBnClickedAutoStart)
END_MESSAGE_MAP()


// CSaveKeyboardDlg 消息处理程序

BOOL CSaveKeyboardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//初始化日志接收模块
	int nPort = 2017;
	while (!m_LogRecv.StartReceive(++nPort, LogCallback, this, "127.0.0.1", "", false) && nPort < 2112){}
	CZLog::SetPort(nPort);

	// 门限设置
	m_nTime = AfxGetApp()->GetProfileInt("Config", "TimeGate", 150);
	SetDlgItemInt(IDC_TIME, m_nTime);
	Log1("连击门限为：%d毫秒", m_nTime);

	// 启动定时器（用于同步日志）
	SetTimer(1, 100, NULL);

	// 启动键盘钩子
	StartHook(CSaveKeyboardDlg::OnKeyboard, this);

	// 初始化系统托盘数据
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nid.hWnd = GetSafeHwnd();
	_stprintf_s(m_nid.szTip, _T("%s"), _T("拯救你的键盘"));
	m_nid.uCallbackMessage = WM_MSG_NOTIFY;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE |NIF_TIP;
	m_nid.uID = IDR_MAINFRAME;

	// 显示托盘图标
	Shell_NotifyIcon(NIM_ADD, &m_nid);

	// 保存当前实例句柄
	if (NULL == g_hWnd)
	{
		g_hWnd = m_hWnd;
	}
	else
	{
		return TRUE;
	}


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

	//如果对话框被最小化，隐藏所有视频窗口，否则显示所有视频窗口
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		ShowToolTip("软件将在后台默默进行监控，阻止您的键盘发生连击。", "拯救连击的键盘", 0, 300);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSaveKeyboardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSaveKeyboardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSaveKeyboardDlg::OnBnClickedOk()
{
	int nTime = GetDlgItemInt(IDC_TIME);
	if (nTime < 1 || nTime > 10000)
	{
		AfxMessageBox("请输入合法的时间间隔(1~10000)！", MB_ICONINFORMATION);
		GetDlgItem(IDC_TIME)->SetFocus();
		return;
	}
	m_nTime = nTime;
	AfxGetApp()->WriteProfileInt("Config", "TimeGate", m_nTime);
	Log1("连击门限更新为：%d毫秒", m_nTime);
}


void CSaveKeyboardDlg::OnBnClickedCancel()
{
	// 窗口可见的时候弹出对话框询问
	if (IsWindowVisible())
	{
		if (AfxMessageBox("确定要退出吗？", MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1) == IDCANCEL)
		{
			return;
		}
	}

	// 停止钩子程序
	StopHook();

	//退出前删除系统托盘图标
	Shell_NotifyIcon(NIM_DELETE, &m_nid);

	OnCancel();
}

// 键盘按下的回调函数
BOOL WINAPI CSaveKeyboardDlg::OnKeyboard(LPVOID lpContext, WPARAM wParam, LPARAM lParam)
{
	// 最高两位都是0的，才是键盘按下事件，只处理键盘按下事件
	if ((lParam >> 30) != 0)
	{
		return TRUE;
	}

	// 只保留最近的20条按键记录
	CSaveKeyboardDlg *pThis = (CSaveKeyboardDlg *)lpContext;
	if (pThis->m_deqKeys.size() > 20)
	{
		pThis->m_deqKeys.pop_front();
	}

	// 检查队列中的内容，如果某个键和上次按下的时间差在制定的时间范围之内，那么视为连击
	DWORD dwTick = pThis->Tick();
	for (size_t i=0; i<pThis->m_deqKeys.size(); i++)
	{
		const Node & n = pThis->m_deqKeys[pThis->m_deqKeys.size() - i - 1];
		if (wParam == n.wKey)
		{
			if (dwTick - n.dwTick < 10)
			{
				// 在chrome等编辑器中会自动每个键出现两次
				return TRUE;
			}
			else if (dwTick - n.dwTick < (DWORD)pThis->m_nTime)
			{
				Log2("<%c>键发生了连击，连击时间为：%d毫秒", wParam, dwTick - n.dwTick);
				return FALSE;
			}
		}
	}

	// 如果是正常输入，将新的按键加入到队列中
	Node node;
	node.wKey = wParam;
	node.dwTick = dwTick;
	pThis->m_deqKeys.push_back(node);

	return TRUE;
}

void CSaveKeyboardDlg::AddLog()
{
	//获取当前待处理数据
	static std::vector<LogNode> vec;
	m_csLog.Lock();
	vec.swap(m_vecLog);
	m_csLog.Unlock();
	if (vec.size() == 0)
	{
		return;
	}

	//保存选中位置
	CHARRANGE cr;
	m_ctrlLog.GetSel(cr);

	//设置待输出文本格式
	CHARFORMAT cf = {0};
	cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
	cf.yHeight	= 180;
	sprintf_s(cf.szFaceName, "宋体");
	m_ctrlLog.SetSel(-1, -1);
	COLORREF aColor[] = 
	{
		RGB(0, 0, 255),
		RGB(0, 150, 0),
		RGB(112, 48, 160),
	};

	//逐一处理日志信息
	for (size_t i=0; i<vec.size(); i++)
	{
		LogNode &node = vec[i];

		//删除多余的行
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

		//插入时间信息
		cf.crTextColor = aColor[0];
		m_ctrlLog.SetSelectionCharFormat(cf);
		m_ctrlLog.ReplaceSel(CTime::GetCurrentTime().Format("%m-%d %H:%M:%S  "));

		//插入日志内容
		cf.crTextColor = aColor[node.nType1 % 3];
		m_ctrlLog.SetSelectionCharFormat(cf);
		m_ctrlLog.ReplaceSel(node.strLog);
		m_ctrlLog.ReplaceSel("\r\n");

		//如果当前窗口被隐藏了，则需要显示消息提示
		if (!IsWindowVisible() && node.strLog.Find("键发生了连击") >= 0)
		{
			ShowToolTip(node.strLog, "连击提示", 0, 100);
		}
	}
	vec.clear();

	//设定光标位置
	if (cr.cpMin != cr.cpMax)
	{
		//恢复用户选中位置
		m_ctrlLog.SetSel(cr);
	}
	else
	{
		//滚动到最新位置
		m_ctrlLog.SendMessage(WM_VSCROLL,SB_BOTTOM, 0);
	}

}

//日志信息回调函数
void CSaveKeyboardDlg::LogCallback(void * lpContext, const void *pData, int nLen, const char * lpszIP)
{
	CSaveKeyboardDlg *pThis = (CSaveKeyboardDlg *)lpContext;
	LogNode node;
	char *pTmp = (char *)pData;

	//读取类型1
	memcpy(&node.nType1, pTmp, 4);
	pTmp += 4;
	nLen -= 4;

	//读取类型2
	memcpy(&node.nType2, pTmp, 4);
	pTmp += 4;
	nLen -= 4;

	//读取字符串长度
	WORD wLen = 0;
	memcpy(&wLen, pTmp, 2);
	pTmp += 2;
	nLen -= 2;

	//剩下的内容应该全部是字符串内容
	if (wLen != (WORD)nLen)
	{
		ASSERT(0);
		return;
	}

	//读取字符串
	node.strLog.Append(pTmp, nLen);

	TRACE("<%d, %d>%s\n", node.nType1, node.nType2, node.strLog);

	//将字符串加入列表
	pThis->m_csLog.Lock();
	if ((int)pThis->m_vecLog.size() >= 500)
	{
		pThis->m_vecLog.clear();
	}
	pThis->m_vecLog.push_back(node);
	pThis->m_csLog.Unlock();

}

// 定时器函数，用于同步日志信息
void CSaveKeyboardDlg::OnTimer(UINT_PTR nIDEvent)
{
	AddLog();
	CDialog::OnTimer(nIDEvent);
}

// 系统托盘的消息回调函数
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

// 确保窗口初始化时不显示
void CSaveKeyboardDlg::OnNcPaint()
{
	static int i = 2;
	if(i > 0)
	{
		i --;
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialog::OnNcPaint();
	}
}

// 显示文字提示
BOOL CSaveKeyboardDlg::ShowToolTip(LPCTSTR szMsg,LPCTSTR szTitle,DWORD dwInfoFlags,UINT uTimeout)
{
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.uFlags = NIF_INFO;
	m_nid.uVersion = NOTIFYICON_VERSION;
	m_nid.uTimeout = uTimeout;
	m_nid.dwInfoFlags = dwInfoFlags;
	strcpy_s( m_nid.szInfoTitle, szTitle );//气泡标题
	strcpy_s( m_nid.szInfo,      szMsg     );//气泡内容
	return Shell_NotifyIcon( NIM_MODIFY, &m_nid);
}

// 关闭文字提示
BOOL CSaveKeyboardDlg::CloseToolTip()
{
	m_nid.cbSize=sizeof(NOTIFYICONDATA);
	m_nid.uFlags = NIF_INFO;
	strcpy_s( m_nid.szInfo,      _T("")     );//气泡内容 设置为空即可取消气泡提示
	return Shell_NotifyIcon( NIM_MODIFY, &m_nid );
}


// 设置开机自动启动
void CSaveKeyboardDlg::OnBnClickedAutoStart()
{
	CButton *pButton = (CButton *)GetDlgItem(IDC_AUTO_START);
	if (!SetAutoStart(pButton->GetCheck()))
	{
		pButton->SetCheck(!pButton->GetCheck());
		AfxMessageBox("设置软件开机自动启动未能成功。请在保证防火墙不会对本软件的该行为进行拦截的情况下重试。", MB_ICONINFORMATION);
	}
}

// 设置是否开机自动启动
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

