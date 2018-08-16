#pragma once

#define FIND_PORT 1001

#define FIND_REQ 0x01
#define FIND_ANS 0x02

class CEvacDLSDlg;
class CFindSocket : public CSocket
{
public:
	CFindSocket(CEvacDLSDlg * _pMainDLg);
	virtual ~CFindSocket();
	virtual void OnReceive(int nErrorCode);
	void SendFindBroadcast();
	void AddCLient(UINT ip);

	CEvacDLSDlg * m_pMainDLg;
	CList<UINT,UINT> m_uClientList;
	BOOL m_bIsServer;

};


