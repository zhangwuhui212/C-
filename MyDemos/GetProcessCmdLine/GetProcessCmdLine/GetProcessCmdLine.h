
// GetProcessCmdLine.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGetProcessCmdLineApp:
// �йش����ʵ�֣������ GetProcessCmdLine.cpp
//

class CGetProcessCmdLineApp : public CWinApp
{
public:
	CGetProcessCmdLineApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGetProcessCmdLineApp theApp;