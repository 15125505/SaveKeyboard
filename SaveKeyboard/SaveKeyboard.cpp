// SaveKeyboard.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "SaveKeyboard.h"
#include "SaveKeyboardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern HWND g_hWnd;


// CSaveKeyboardApp

BEGIN_MESSAGE_MAP(CSaveKeyboardApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSaveKeyboardApp 构造

CSaveKeyboardApp::CSaveKeyboardApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSaveKeyboardApp 对象

CSaveKeyboardApp theApp;


// CSaveKeyboardApp 初始化

BOOL CSaveKeyboardApp::InitInstance()
{
	CWinApp::InitInstance();

	// 防止启动多个实例
	if (NULL != g_hWnd)
	{
		::ShowWindow(g_hWnd, SW_SHOW);
		::ShowWindow(g_hWnd, SW_RESTORE);
		::SetForegroundWindow(g_hWnd);
		return FALSE;
	}

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("ZF"));

	AfxInitRichEdit2();

	CSaveKeyboardDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
