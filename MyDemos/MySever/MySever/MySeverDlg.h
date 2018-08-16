
// MySeverDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "SrvListen.h"
#include "SrvSocketCmd.h"
#include "SrvSocketData.h"

#define WM_MY_PROGRESS (WM_USER + 1)

typedef enum
{
	BROADCAST_PORT_CLIENT		=5220,
	BROADCAST_PORT_PAD			=5221,
	SERVER_PORT_COMMAND			=5222,
	SERVER_PORT_DATA			=5223,
}SERVERPORT;

class CPushFile;
// CMySeverDlg 对话框
class CMySeverDlg : public CDialogEx
{
// 构造
public:
	CMySeverDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYSEVER_DIALOG };

	char * GetHostIp();
	void EnabelServerElement(BOOL btrue);
	int StartServer();
	int StopServer();

	void OnAccept(int nErrorCode,UINT Port);
	void OnSrvCloseCommand(CMySocket * pSocket, int nErrorCode);

	
	void OnClientConnect(MYNODE node,UINT Port);
	void SrvSendMsg(CString str);
	void PrintMsg(CString str);
	int  PushFile();
	void OnSchedulePutFile();
	
	void OnProgress(CMySocket * pSocket);

	void OnSocketCommand(CMySocket * pSocket, MYMSG msg);
	
	void OnPushFileAck(CMySocket * pSocket, MYMSG msg);
	void PushFileEnd(int error , CMySocket * pSocket);
	void OnPushFileEndAck(CMySocket * pSocket, MYMSG msg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	CSrvListen* m_pSocketCommand;
	CSrvListen* m_pSocketData;
	CArray<CSrvSocketCmd*,CSrvSocketCmd*> m_ClientCmd;
	CArray<CSrvSocketData*,CSrvSocketData*> m_ClientData;

	CArray<CPushFile, CPushFile> m_PushArray;
	CPushFile * m_pCurPushFile;
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
	CListBox m_list;
	CEdit m_msgEdit;
	CMFCEditBrowseCtrl m_fileEdit;
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDownloadFile();
	afx_msg LRESULT OnUpdateProgress(WPARAM wp ,LPARAM lp);
	int m_port;
	DWORD m_ip;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};

class CPushFile
{
public:
	CStringA m_file;
	UINT m_addr;
	UINT m_port;
	UINT m_State;
	ULONGLONG m_TotalSize;
	UINT m_CommSize;

	CPushFile(){};
	CPushFile(CStringA f, UINT addr, UINT port)
	{
		m_file = f;
		m_addr = addr;
		m_port = port;
		m_State = 0;
		m_TotalSize = 0;
		m_CommSize = 0;
	};
};
