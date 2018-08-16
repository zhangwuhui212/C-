// TftpServer.cpp : implementation file
//

#include "stdafx.h"
#include "EvacDLS.h"
#include "TftpSrvListen.h"
#include "EvacDLSDlg.h"

// CTftpServer

CTftpSrvListen::CTftpSrvListen(CEvacDLSDlg * pMainDlg)
{
	m_pMainDlg = pMainDlg;
}

CTftpSrvListen::~CTftpSrvListen()
{
}


// CTftpServer member functions


void CTftpSrvListen::OnReceive(int nErrorCode)
{
	int len  = 0 , sour_len = 0;
	char data[1024] = {0};
	sockaddr_in sour_addr;
	sour_addr.sin_family = AF_INET;
	sour_addr.sin_port = htons(INADDR_ANY);
	sour_len = sizeof(sockaddr);
	//sour_addr.sin_addr.s_addr = inet_addr(desthost); // Ô¶³ÌIPµØÖ·
	len = ReceiveFrom(data,1024,(sockaddr *)&sour_addr,&sour_len);
	if (len > 0)
	{
		m_pMainDlg->OnReceiveTftpData(this,sour_addr,data,len);
	}
	CSocket::OnReceive(nErrorCode);
}
