#include "StdAfx.h"
#include "MyClient.h"
#include "MyClientDlg.h"
#include "CltSocketCmd.h"


CCltSocketCmd::CCltSocketCmd(UINT Port)
{
	m_Port = Port;
}


CCltSocketCmd::~CCltSocketCmd(void)
{
}

void CCltSocketCmd::SetSocketInfo(CString name/* = L""*/)
{
	SOCKADDR_IN sa;
	int len = sizeof(sa);
	GetSockName((SOCKADDR*)&sa, &len);
	m_node.addr = ntohl(sa.sin_addr.S_un.S_addr);
	m_node.port = ntohs(sa.sin_port);
}

void CCltSocketCmd::OnReceive(int nErrorCode)
{
	MYMSG head;
	UINT len = Receive(&head, sizeof(head));
	if (len == sizeof(MYMSG))
	{
		((CMyClientDlg *)(theApp.m_pMainWnd))->OnSocketCommand(this, head);
	}

	CSocket::OnReceive(nErrorCode);
}

void CCltSocketCmd::OnClose(int nErrorCode)
{
	((CMyClientDlg *)(theApp.m_pMainWnd))->OnSrvClosed(nErrorCode,m_Port);
	CSocket::OnClose(nErrorCode);
}

int CCltSocketCmd::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	ASSERT(nBufLen == sizeof(MYMSG));
	MYMSG msg = *(MYMSG*)lpBuf;

	//htonmsg(msg);

	return CSocket::Send(&msg, nBufLen, nFlags);
}


