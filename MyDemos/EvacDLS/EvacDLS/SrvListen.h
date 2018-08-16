#pragma once

// CSrvListen command target
class CEvacDLSDlg;
class CSrvListen : public CSocket
{
public:
	CSrvListen(CEvacDLSDlg * pMainDlg);
	virtual ~CSrvListen();

	CEvacDLSDlg * m_pMainDlg;
	virtual void OnAccept(int nErrorCode);
};


