#pragma once
#include "mysocket.h"
class CSrvListen :public CMySocket
{
public:
	CSrvListen(UINT Port);
	~CSrvListen(void);

	virtual void SetSocketInfo(CString name = L"");
	virtual void OnAccept(int nErrorCode);
};

