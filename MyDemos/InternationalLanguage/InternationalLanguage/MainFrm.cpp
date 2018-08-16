
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "InternationalLanguage.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

#define ID_LANGUAGE_FIRST 0xE12D
#define ID_LANGUAGE_LAST  0xE515

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_SETTING_LANGUAGE, &CMainFrame::OnSettingLanguage)
	ON_UPDATE_COMMAND_UI(ID_SETTING_LANGUAGE, &CMainFrame::OnUpdateSettingLanguage)
	ON_COMMAND_RANGE(ID_LANGUAGE_FIRST, ID_LANGUAGE_LAST, &CMainFrame::OnSwitchLanguage)
	ON_UPDATE_COMMAND_UI_RANGE(ID_LANGUAGE_FIRST, ID_LANGUAGE_LAST, &CMainFrame::OnUpdateLanguageSelect)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	FindLanguagesDLl();
	LANGID dsid = GetUiLanguage();
	LoadResourceDLL(dsid);
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC);
	//m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(FALSE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);


	// 启用工具栏和停靠窗口菜单替换
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);
	lstBasicCommands.AddTail(ID_SETTING_LANGUAGE);
	

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建类视图
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“类视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnSettingLanguage()
{
	
}


void CMainFrame::OnUpdateSettingLanguage(CCmdUI *pCmdUI)
{
	
}


BOOL CMainFrame::OnShowPopupMenu(CMFCPopupMenu* pMenuPopup)
{
	 BOOL bRes = CFrameWndEx::OnShowPopupMenu(pMenuPopup);

	 if (pMenuPopup == NULL)
	 {
		 return TRUE;
	 }

	 if (pMenuPopup != NULL && !pMenuPopup->IsCustomizePane())
	 {
		 AdjustLanguageMenu(pMenuPopup, ID_ALL_LANGUAGE);
	 }

// 	 int iIndex = -1;
// 	 if (!CMFCToolBar::IsCustomizeMode() &&
// 		 (iIndex = pMenuPopup->GetMenuBar ()->CommandToIndex (ID_SETTING_LANGUAGE)) >= 0)
// 	 {
// 		 // remove the {Dynamic Command} item in noncustomize mode
// 		 //pMenuPopup->RemoveItem (iIndex);
// 		 pMenuPopup->InsertSeparator (iIndex); // insert the separator at the end
// 		 // IDS_EDIT_MYITEM_1 and IDS_EDIT_MYITEM_1 should be defined in the string table
// 		 // for status text and tooltip
// 		 pMenuBar->GetButton (iIndex);
// 		 pMenuPopup->InsertItem (
// 			 CMFCToolBarMenuButton (ID_LANGUAGE_FIRST, NULL, -1, _T("&MyItem 1")), iIndex + 1);
// 		 pMenuPopup->InsertItem (
// 			 CMFCToolBarMenuButton (ID_LANGUAGE_LAST, NULL, -1, _T("MyItem &2")), iIndex + 2);
// 
// 		 // don't forget to add message handlers (ON_COMMAND) to the message map
// 	 }
	 return bRes;
}
void CMainFrame::AdjustLanguageMenu (CMFCPopupMenu* pMenuPopup, UINT uiID)
{
	CMFCPopupMenuBar* pMenuBar = pMenuPopup->GetMenuBar ();
	ASSERT (pMenuBar != NULL);
	if (pMenuBar == NULL)
	{
		return;
	}

	int iIndex = pMenuBar->CommandToIndex (uiID);
	if (iIndex < 0)
	{
		return;
	}
#if 0
	//type 1 :
	CMFCToolBarMenuButton* pColorButton = new CMFCToolBarMenuButton(ID_LANGUAGE_FIRST, NULL, -1, _T("&MyItem 1"));

	pMenuBar->ReplaceButton (uiID, *pColorButton, TRUE);
	delete pColorButton;
#endif

#if 1
	//type 2:
	pMenuPopup->RemoveItem (iIndex);
	pMenuPopup->InsertSeparator (iIndex); 

	CString strLangName;
	int i, nLangCount = (int)m_LangArray.GetSize();
	for (i = 0; i < nLangCount; i++)
	{
		GetLocaleInfo(m_LangArray[i], LOCALE_SNATIVELANGNAME, strLangName.GetBufferSetLength(MAX_PATH), MAX_PATH);
		strLangName.ReleaseBuffer();
		pMenuPopup->InsertItem (
			CMFCToolBarMenuButton (ID_LANGUAGE_FIRST + i, NULL, -1, strLangName), iIndex);
	}

// 	pMenuPopup->InsertItem (
// 		CMFCToolBarMenuButton (ID_LANGUAGE_FIRST, NULL, -1, _T("&MyItem 1")), iIndex);
// 	pMenuPopup->InsertItem (
// 		CMFCToolBarMenuButton (ID_LANGUAGE_LAST, NULL, -1, _T("MyItem &2")), iIndex+1);
#endif



}


void CMainFrame::AdjustColorsMenu (CMFCPopupMenu* pMenuPopup, UINT uiID)
{
	CMFCPopupMenuBar* pMenuBar = pMenuPopup->GetMenuBar ();
	ASSERT (pMenuBar != NULL);
	if (pMenuBar == NULL)
	{
		return;
	}

	int iIndex = pMenuBar->CommandToIndex (uiID);
	if (iIndex < 0)
	{
		return;
	}

	if (DYNAMIC_DOWNCAST (CMFCColorMenuButton, pMenuBar->GetButton (iIndex)) != NULL)
	{
		return;
	}

	CMFCColorMenuButton* pColorButton = new CMFCColorMenuButton();
	pMenuBar->ReplaceButton (ID_CHAR_COLOR, *pColorButton, TRUE);
	delete pColorButton;
}

void CMainFrame::OnSwitchLanguage(UINT uId)
{
	uId -= ID_LANGUAGE_FIRST;
	if(uId >= (UINT)m_LangArray.GetSize())
		return ;
	if(m_LangArray[uId] != m_nCurLangId)
	{
		m_nCurLangId = m_LangArray[uId];
		OnSwitchUILangbyID();
	}
}

void CMainFrame::OnUpdateLanguageSelect(CCmdUI *pCmdUI)
{
	UINT nCurrentItem = 0;
	int i, nLangCount = (int)m_LangArray.GetSize();
	for (i = 0; i < nLangCount; i++)
	{
		if (m_nCurLangId == m_LangArray[i])
			nCurrentItem = ID_LANGUAGE_FIRST + i;
	}
	if(nCurrentItem != 0)
		pCmdUI -> SetRadio(nCurrentItem == pCmdUI->m_nID);
}

BOOL CMainFrame::OnSwitchUILangbyID()
{
	if(m_hResourceHandle)
		FreeLibrary(m_hResourceHandle);
	m_hResourceHandle = NULL;
	if(m_nCurLangId != DEFAULTLANGID)
	{
		TCHAR strDll[MAX_PATH];
		GetModuleFileName(NULL, strDll, MAX_PATH);
		wsprintf(strDll + wcslen(strDll) - 4, TEXT("_%d.dll"), m_nCurLangId);

		m_hResourceHandle = LoadLibrary(strDll);
		if(NULL == m_hResourceHandle)
		{
			//AfxMessageBox(_T("ERROR"));
			m_nCurLangId = DEFAULTLANGID;
		}
	}
	theApp.WriteInt(_T("LanguageId"), m_nCurLangId);

	if(NULL == m_hResourceHandle)
		m_hResourceHandle = m_hDefaultResourceHandle;

	if(m_hResourceHandle)
	{
		AfxSetResourceHandle(m_hResourceHandle);
		m_wndMenuBar.RestoreOriginalstate();

		CString strViewName;
		if(strViewName.LoadString(IDR_MAINFRAME))
			SetWindowText(strViewName);
		if(strViewName.LoadString(AFX_IDS_IDLEMESSAGE))
			m_wndStatusBar.SetPaneText(0, strViewName);		
		int k, nIndi = sizeof(indicators) / sizeof(UINT);
		for(k = 1; k < nIndi; k++)
		{
			if(strViewName.LoadString(indicators[k]))
				m_wndStatusBar.SetPaneText(k, strViewName);
		}
		return TRUE;
	}
	return FALSE;
}

int CMainFrame::FindLanguagesDLl() {
	TCHAR				CurrentDirectory[MAX_PATH];
	TCHAR				ShortDirectory[MAX_PATH];
	int					nLangId;
	TCHAR				strLangName[MAX_PATH];
	WIN32_FIND_DATA		FindFileData;
	HANDLE				hFind;
	int					i = 0;
	int nOffset ;
	CString fileName;
	if( GetModuleFileName(NULL, CurrentDirectory,MAX_PATH)) {
		::PathRemoveExtension(CurrentDirectory);
		//wcscpy_s(ShortDirectory,CurrentDirectory);
		_tcscpy_s(ShortDirectory, sizeof(ShortDirectory)/sizeof(TCHAR), CurrentDirectory);
		if( _tcsclen(CurrentDirectory) < MAX_PATH-4 )
			_tcscat_s(CurrentDirectory, sizeof(CurrentDirectory)/sizeof(TCHAR), _T("_*.dll"));
		fileName = ::PathFindFileName(ShortDirectory);
		nOffset = wcslen(fileName) ;
	}
	else
		return 0;
	hFind = FindFirstFile(CurrentDirectory, &FindFileData);
	if( hFind == INVALID_HANDLE_VALUE ) {
		FindClose(hFind);
		return 0;
	}
	m_LangArray.RemoveAll();
	do {
		if( FindFileData.dwFileAttributes ) {
			nLangId = _ttoi(FindFileData.cFileName + nOffset + 1);
			if( nLangId ) {
				if( GetLocaleInfo(nLangId,LOCALE_SNATIVELANGNAME,strLangName,MAX_PATH)) {
					m_LangArray.Add(nLangId);
					i++;
				}
			}
		}
	} while ( FindNextFile(hFind, &FindFileData) );
	FindClose(hFind);
	return i;
}

HMODULE	CMainFrame::LoadResourceDLL(LANGID DesiredLanguage) {
	TCHAR		strDll[MAX_PATH];
	HMODULE		hFind;

	if( GetModuleFileName(NULL, strDll,MAX_PATH)) {
		::PathRemoveExtension(strDll);
		CString str;
		str.Format(TEXT("_%d.dll"), DesiredLanguage);
		_tcscat_s(strDll, sizeof(strDll)/sizeof(TCHAR), str);
	}
	else
		return AfxGetResourceHandle();

	hFind = LoadLibrary(strDll);
	if (hFind)
		return hFind;
	else
		return AfxGetResourceHandle();
}

LANGID CMainFrame::GetUiLanguage() {
	LANGID uiLangID = GetUserDefaultUILanguage();

	if (uiLangID == 0) {
		uiLangID = GetUserDefaultLangID();
	}
	// Return the found language ID.
	return (uiLangID);
}
