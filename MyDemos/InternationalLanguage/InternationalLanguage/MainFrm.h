
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:
	void AdjustLanguageMenu (CMFCPopupMenu* pMenuPopup, UINT uiID);
	void AdjustColorsMenu (CMFCPopupMenu* pMenuPopup, UINT uiID);

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	enum{
		BUFSIZE			= 1024,
		DEFAULTLANGID	= MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	};
	HMODULE			m_hResourceHandle;
	HINSTANCE		m_hDefaultResourceHandle;
	CUIntArray		m_LangArray;
	LCID			m_nCurLangId;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnUpdateLanguageSelect(CCmdUI *pCmdUI);
	afx_msg void OnSwitchLanguage(UINT);
	DECLARE_MESSAGE_MAP()

	BOOL OnSwitchUILangbyID();
	
	LANGID GetUiLanguage();
	HMODULE	LoadResourceDLL(LANGID DesiredLanguage);
	int FindLanguagesDLl();

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnSettingLanguage();
	afx_msg void OnUpdateSettingLanguage(CCmdUI *pCmdUI);
	virtual BOOL OnShowPopupMenu(CMFCPopupMenu* pMenuPopup);
};


