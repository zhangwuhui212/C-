#include "StdAfx.h"
#include "MySever.h"
#include "MySeverDlg.h"
#include "SrvListen.h"


CSrvListen::CSrvListen(UINT Port)
{
	m_Port = Port;
}

CSrvListen::~CSrvListen(void)
{
}

void CSrvListen::SetSocketInfo(CString name/* = L""*/)
{
	//strcpy_s(m_node.name, "Server Listen");
}

void CSrvListen::OnAccept(int nErrorCode)
{
	((CMySeverDlg *)(theApp.m_pMainWnd))->OnAccept(nErrorCode,m_Port);
	CSocket::OnAccept(nErrorCode);
}
