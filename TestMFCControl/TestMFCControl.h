
// TestMFCControl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestMFCControlApp: 
// �йش����ʵ�֣������ TestMFCControl.cpp
//

class CTestMFCControlApp : public CWinApp
{
public:
	CTestMFCControlApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestMFCControlApp theApp;
