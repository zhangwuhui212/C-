// FindSocket.cpp : implementation file
//

#include "stdafx.h"
#include "EvacDLS.h"
#include "FindSocket.h"
#include "EvacDLSDlg.h"

// CFindSocket

CFindSocket::CFindSocket(CEvacDLSDlg * pMainDLg)
{
	m_pMainDLg = pMainDLg;
	m_bIsServer = FALSE;
	m_uClientList.RemoveAll();
}

CFindSocket::~CFindSocket()
{
	m_uClientList.RemoveAll();
}


// CFindSocket member functions


void CFindSocket::OnReceive(int nErrorCode)
{
	int len  = 0 , addr_len = 0;
	char data[1024] = {0};
	sockaddr_in addrfrom;
	addrfrom.sin_family		= AF_INET;
	addrfrom.sin_addr.s_addr= htonl(INADDR_BROADCAST);
	addrfrom.sin_port		= htons(FIND_PORT);
	addr_len = sizeof(sockaddr);
	len = ReceiveFrom(data,1024,(sockaddr *)&addrfrom,&addr_len);
	if (len > 0)
	{
		m_pMainDLg->OnReceiveFindData(this,addrfrom,data,len);
	}
	
	CSocket::OnReceive(nErrorCode);
}

void CFindSocket::SendFindBroadcast()
{
	char sFindBroadcast[64] = "";
	sockaddr_in addrto; 
	int addr_len = sizeof(sockaddr_in);
	m_bIsServer = TRUE;
	addrto.sin_family		= AF_INET;  
	addrto.sin_addr.s_addr	= htonl(INADDR_BROADCAST);
	addrto.sin_port			= htons(FIND_PORT);
	sFindBroadcast[0]  = FIND_REQ;
	sFindBroadcast[1]  = 0x00;
	sFindBroadcast[2]  = 0x10;
	SendTo(sFindBroadcast,strlen(sFindBroadcast),(sockaddr *)&addrto,addr_len);
}

void CFindSocket::AddCLient(UINT ip)
{
	if (ip == inet_addr("192.168.16.21"))
	{
		return;
	}
	if (m_uClientList.Find(ip,0)>0)
	{
		return;
	}
	m_uClientList.AddTail(ip);
}
