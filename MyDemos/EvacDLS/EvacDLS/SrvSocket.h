#pragma once

// CSrvSocket command target
struct m_client_info
{
	UINT  state;
	DWORD addr;
	UINT  port;
	DWORD tc;
};//sClientInfo;


class CEvacDLSDlg;
class CSrvSocket : public CSocket
{
public:
	CSrvSocket(CEvacDLSDlg * pMainDlg);
	virtual ~CSrvSocket();

	CEvacDLSDlg * m_pMainDlg;
	//virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	struct m_client_info m_node;
};


