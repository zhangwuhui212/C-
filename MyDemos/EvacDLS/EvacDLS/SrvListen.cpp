// SrvListen.cpp : implementation file
//

#include "stdafx.h"
#include "EvacDLS.h"
#include "SrvListen.h"
#include "EvacDLSDlg.h"


// CSrvListen

CSrvListen::CSrvListen(CEvacDLSDlg * pMainDlg)
{
	m_pMainDlg = pMainDlg;
}

CSrvListen::~CSrvListen()
{
}


// CSrvListen member functions


void CSrvListen::OnAccept(int nErrorCode)
{
	m_pMainDlg->OnAcceptClient(this,nErrorCode);
	CSocket::OnAccept(nErrorCode);
}
