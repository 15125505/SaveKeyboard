// KeyHook.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "KeyHook.h"

//data_seg�еı�������֤�ڽ��̼乲����ڴ�
#pragma data_seg("KeyHook") 
//HWND		g_hParent = NULL;
//int			g_nMsgID = 0;
//int			g_nMouseFlag = 0;
#pragma data_seg() 
	
HHOOK		g_hKeyHook = NULL;				// ���̹���
HINSTANCE	g_hMod = NULL;					// ϵͳInstance
KeyboardCallback g_pfnCallback = NULL;		// �ص�����
LPVOID g_lpContext = NULL;					// ��������


// ���̹���ʵ�ʴ�����
LRESULT WINAPI KeyHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	if (nCode == HC_ACTION && NULL != g_pfnCallback) 
	{
		if (!g_pfnCallback(g_lpContext, wp, lp))
		{
			return TRUE;
		}
	}
	return CallNextHookEx(g_hKeyHook, nCode, wp, lp);
}


// �������ӳ���
KEYHOOK_API BOOL StartHook(KeyboardCallback pfnCallback, LPVOID lpContext)
{
	// �������̹���
	if (g_hKeyHook != NULL)
	{
		return FALSE;
	}
	g_hKeyHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, KeyHookProc, g_hMod, 0);
	if (g_hKeyHook == NULL) 
	{
		return FALSE;
	}
	g_pfnCallback = pfnCallback;
	g_lpContext = lpContext;
	return TRUE;
}

// ֹͣ���ӳ���
KEYHOOK_API BOOL StopHook(void)
{
	return UnhookWindowsHookEx(g_hKeyHook);
}


