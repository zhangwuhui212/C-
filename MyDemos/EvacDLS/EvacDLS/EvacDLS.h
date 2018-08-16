
// EvacDLS.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CEvacDLSApp:
// See EvacDLS.cpp for the implementation of this class
//

class CEvacDLSApp : public CWinApp
{
public:
	CEvacDLSApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CEvacDLSApp theApp;