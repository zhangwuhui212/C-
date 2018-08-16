
// MyClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


#include "resource.h"
#include "CltSocketCmd.h"
#include "CltSocketData.h"

#define WM_MY_PROGRESS (WM_USER + 1)

typedef enum
{
	BROADCAST_PORT_CLIENT		=5220,
	BROADCAST_PORT_PAD			=5221,
	SERVER_PORT_COMMAND			=5222,
	SERVER_PORT_DATA			=5223,
}SERVERPORT;

// CMyClientDlg �Ի���
class CMyClientDlg : public CDialogEx
{
// ����
public:
	CMyClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYCLIENT_DIALOG };

	char * GetHostIp();
	void EnabelServerElement(BOOL btrue);

	int ConnectServer();
	int StopConnect();

	void OnSrvClosed(int nErrorCode,UINT Port);
	void OnSocketCommand(CMySocket * pSocket, MYMSG msg);
	void PrintMsg(CString str);

	void OnProgress(CMySocket * pSocket);

	void OnPushFile(CMySocket * pSocket, MYMSG msg);
	void OnPushFileEnd(CMySocket * pSocket, MYMSG msg);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	CCltSocketCmd* m_pSocketCommand;
	CCltSocketData* m_pSocketData;
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
	CListBox m_list;
	CEdit m_msgEdit;
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnUpdateProgress(WPARAM wp ,LPARAM lp);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	DWORD m_ip;
	int m_port;
};
