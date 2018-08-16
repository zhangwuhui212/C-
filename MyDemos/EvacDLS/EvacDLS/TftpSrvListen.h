#pragma once

// CTftpServer command target
class CEvacDLSDlg;
class CTftpSrvListen : public CSocket
{
public:
	CTftpSrvListen(CEvacDLSDlg * pMainDlg);
	virtual ~CTftpSrvListen();


	CEvacDLSDlg * m_pMainDlg;
	virtual void OnReceive(int nErrorCode);
};


