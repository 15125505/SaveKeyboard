// SaveKeyboard.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSaveKeyboardApp:
// �йش����ʵ�֣������ SaveKeyboard.cpp
//

class CSaveKeyboardApp : public CWinApp
{
public:
	CSaveKeyboardApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSaveKeyboardApp theApp;