
// MFCRedirectDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CMFCRedirectDlg �Ի���
class CMFCRedirectDlg : public CDialogEx
{
// ����
public:
	CMFCRedirectDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCREDIRECT_DIALOG };
	BOOL RunCMD();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CListBox m_list;
};
