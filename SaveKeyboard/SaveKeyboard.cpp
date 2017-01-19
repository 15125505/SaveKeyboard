// SaveKeyboard.cpp : ����Ӧ�ó��������Ϊ��
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


// CSaveKeyboardApp ����

CSaveKeyboardApp::CSaveKeyboardApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSaveKeyboardApp ����

CSaveKeyboardApp theApp;


// CSaveKeyboardApp ��ʼ��

BOOL CSaveKeyboardApp::InitInstance()
{
	CWinApp::InitInstance();

	// ��ֹ�������ʵ��
	if (NULL != g_hWnd)
	{
		::ShowWindow(g_hWnd, SW_SHOW);
		::ShowWindow(g_hWnd, SW_RESTORE);
		::SetForegroundWindow(g_hWnd);
		return FALSE;
	}

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("ZF"));

	AfxInitRichEdit2();

	CSaveKeyboardDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
