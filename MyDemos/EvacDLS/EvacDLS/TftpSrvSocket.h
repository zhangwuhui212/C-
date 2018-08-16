#pragma once

#include "TftpComm.h"

class CEvacDLSDlg;
class CTftpSrvSocket : public CSocket
{
public:
	CTftpSrvSocket(CEvacDLSDlg * pMainDlg);
	virtual ~CTftpSrvSocket();
	virtual void OnReceive(int nErrorCode);
	int OnGetFile(sockaddr_in sour_addr,char * buf,int len);
	int OnPutFile(sockaddr_in sour_addr,char * buf,int len);
	int OnAbort();

	FILE    *	m_pFile;
	sockaddr_in m_addr;
	TftpInfo	m_tftpInfo;
	CEvacDLSDlg * m_pMainDlg;
};


