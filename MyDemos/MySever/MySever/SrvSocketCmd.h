#pragma once
#include "mysocket.h"
class CSrvSocketCmd :public CMySocket
{
public:
	CSrvSocketCmd(UINT Port);
	~CSrvSocketCmd(void);

	virtual void SetSocketInfo(CString name = L"");
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
};

