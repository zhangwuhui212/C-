
// EvacDLSDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CSrvListen;
class CSrvSocket;
class CTftpSrvListen;
class CTftpSrvSocket;
class CTftpCltSocket;
class CFindSocket;
// CEvacDLSDlg dialog
class CEvacDLSDlg : public CDialogEx
{
// Construction
public:
	CEvacDLSDlg(CWnd* pParent = NULL);	// standard constructor

	void AddClient(CSrvSocket * client);
	POSITION FindClient(CString strIPP);
	void OnAcceptClient(CSrvListen * pListen,int nErrorCode);
	void OnReceiveData(CSrvSocket * pClt,char * data,int len);
	void OnClientClose(CSrvSocket * pClt,int nErrorCode);

	void OnReceiveTftpData(CTftpSrvListen * pClt,sockaddr_in sour_addr,char * data,int len); 

	void OnTftpSrvClose(CTftpSrvSocket * pClt,int nErrorCode);
	void OnTftpCltClose(CTftpCltSocket * pClt,int nErrorCode); 

	void OnReceiveFindData(CFindSocket * pClt,sockaddr_in addr,char * data,int len);

	int FindSrvExist(sockaddr_in addr);

// Dialog Data
	enum { IDD = IDD_EVACDLS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	sockaddr_in m_addr;
	
	CSrvListen * m_pSctListen;
	CSrvSocket * m_pCltSocket;
	CList<CSrvSocket *,CSrvSocket *> m_pClientList;

	CTftpSrvListen * m_pTftpSrvListen;
	CTftpCltSocket * m_pTftpCltSocket;
	CArray<CTftpSrvSocket *,CTftpSrvSocket *> m_pSrvList;


	CFindSocket * m_pFindBroadcast;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listBox_ctrl;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CListCtrl m_clientList_ctrl;
	CIPAddressCtrl m_IP_ctrl;
	CEdit m_port_edit;
	int m_port;
	DWORD m_ip;
	UINT  m_timer;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
};
