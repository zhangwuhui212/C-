#include "StdAfx.h"
#include "MySever.h"
#include "MySeverDlg.h"
#include "SrvSocketCmd.h"


CSrvSocketCmd::CSrvSocketCmd(UINT Port)
{
	m_Port = Port;
}


CSrvSocketCmd::~CSrvSocketCmd(void)
{
}

void CSrvSocketCmd::SetSocketInfo(CString name/* = L""*/)
{
	SOCKADDR_IN sa;
	int len = sizeof(sa);
	GetPeerName((SOCKADDR*)&sa, &len);
	m_node.addr = ntohl(sa.sin_addr.S_un.S_addr);
	m_node.port = ntohs(sa.sin_port);
}

void CSrvSocketCmd::OnReceive(int nErrorCode)
{
	MYMSG msgs[8];
	UINT len = Receive(msgs, sizeof(msgs));
	ASSERT(len == (len/sizeof(MYMSG)*sizeof(MYMSG)));
	for(UINT i=0; i<len/sizeof(MYMSG); i++)
	{
		//ntohmsg(msgs[i]);
		((CMySeverDlg *)(theApp.m_pMainWnd))->OnSocketCommand(this, msgs[i]);
	}

	CSocket::OnReceive(nErrorCode);
}

void CSrvSocketCmd::OnClose(int nErrorCode)
{
	((CMySeverDlg *)(theApp.m_pMainWnd))->OnSrvCloseCommand(this, nErrorCode);
	CSocket::OnClose(nErrorCode);
}

int CSrvSocketCmd::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	ASSERT(nBufLen == sizeof(MYMSG));
	MYMSG msg = *(MYMSG*)lpBuf;

	//htonmsg(msg);

	return CSocket::Send(&msg, nBufLen, nFlags);
}