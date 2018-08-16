
// GetProcessCmdLineDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CGetProcessCmdLineDlg 对话框
class CGetProcessCmdLineDlg : public CDialogEx
{
// 构造
public:
	CGetProcessCmdLineDlg(CWnd* pParent = NULL);	// 标准构造函数

	void LoadProcess();
// 对话框数据
	enum { IDD = IDD_GETPROCESSCMDLINE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_process;
	CListBox m_list_process_info;
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRefresh();
};
