// SrvSocket.cpp : implementation file
//

#include "stdafx.h"
#include "EvacDLS.h"
#include "SrvSocket.h"
#include "EvacDLSDlg.h"


// CSrvSocket

CSrvSocket::CSrvSocket(CEvacDLSDlg * pMainDlg)
{
	m_pMainDlg = pMainDlg;
	m_node.state = 0;
	m_node.addr = 0;
	m_node.port = 5000;
	m_node.tc = 0;
	m_node.tc = GetTickCount();

}

CSrvSocket::~CSrvSocket()
{
}


// CSrvSocket member functions


// int CSrvSocket::Send(const void* lpBuf, int nBufLen, int nFlags)
// {
// 	
// 	return CSocket::Send(lpBuf, nBufLen, nFlags);
// }


void CSrvSocket::OnReceive(int nErrorCode)
{
	char data[1024] = {0};
	int len = Receive(data,1024);
	m_node.tc = GetTickCount();

	if (len > 0)
	{
		m_pMainDlg->OnReceiveData(this,data,len);
	}
	CSocket::OnReceive(nErrorCode);
}


void CSrvSocket::OnClose(int nErrorCode)
{
	m_pMainDlg->OnClientClose(this,nErrorCode);
	CSocket::OnClose(nErrorCode);
}
