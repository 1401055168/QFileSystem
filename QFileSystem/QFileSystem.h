
// QFileSystem.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQFileSystemApp: 
// �йش����ʵ�֣������ QFileSystem.cpp
//

class CQFileSystemApp : public CWinApp
{
public:
	CQFileSystemApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CQFileSystemApp theApp;
