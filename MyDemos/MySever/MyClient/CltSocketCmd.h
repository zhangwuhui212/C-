#pragma once
#include "MySocket.h"
class CCltSocketCmd :public CMySocket
{
public:
	CCltSocketCmd(UINT Port);
	~CCltSocketCmd(void);

	virtual void SetSocketInfo(CString name = L"");
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
};

