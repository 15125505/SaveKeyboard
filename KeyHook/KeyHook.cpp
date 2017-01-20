// KeyHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "KeyHook.h"

//data_seg中的变量将保证在进程间共享该内存
#pragma data_seg("KeyHook") 
//HWND		g_hParent = NULL;
//int			g_nMsgID = 0;
//int			g_nMouseFlag = 0;
#pragma data_seg() 
	
HHOOK		g_hKeyHook = NULL;				// 键盘钩子
HINSTANCE	g_hMod = NULL;					// 系统Instance
KeyboardCallback g_pfnCallback = NULL;		// 回调函数
LPVOID g_lpContext = NULL;					// 环境变量


// 键盘钩子实际处理函数
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


// 启动钩子程序
KEYHOOK_API BOOL StartHook(KeyboardCallback pfnCallback, LPVOID lpContext)
{
	// 启动键盘钩子
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

// 停止钩子程序
KEYHOOK_API BOOL StopHook(void)
{
	return UnhookWindowsHookEx(g_hKeyHook);
}


