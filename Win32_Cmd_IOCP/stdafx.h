// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT	0x0501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//外部库文件引用
#include <afxmt.h>
#include <afx.h>
#include <afxtempl.h>

#include <winsock2.h>
#include <memory>

//外部自定义头文件引用

//#include "Resource.h"
//#include "ICOMPro.h"
//#include "datatype.h"
//#include "Reg.h"
//#include "Debug.h"
//#include "Helper.h"

#if defined(_WIN32_WCE_CEPC) || defined(_ARM_)
#include "AtomCE.h"
#endif


//编译选项

//[!if TRYCONNECT_TRUE]
//#define _TRYCONNECT_TRUE
//[!endif]
//
//[!if TRYCONNECT_USERDEFINE]
//#define _TRYCONNECT_USEDEFINE
//[!endif]
//
//[!if TRYCONNECT_USEPACKET]
//#define _TRYCONNECT_USEPACKET
//[!endif]
//
//[!if REOPENCOM]
//#define _REOPENCOM
//[!endif]



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "lock.h"
