#pragma once

#include "TftpComm.h"

class CEvacDLSDlg;
class CTftpCltSocket : public CSocket
{
public:
	CTftpCltSocket(CEvacDLSDlg * pMainDlg);
	virtual ~CTftpCltSocket();
	virtual void OnReceive(int nErrorCode);
	int GetFile(sockaddr_in sour_addr,char * buf,int len);
	int PutFile(sockaddr_in sour_addr,char * buf,int len);
	int OnAbort();

	FILE    *	m_pFile;
	sockaddr_in m_addr;
	TftpInfo	m_tftpInfo;
	CEvacDLSDlg * m_pMainDlg;
	
};


