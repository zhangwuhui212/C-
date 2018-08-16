#pragma once
#include "mysocket.h"
class CCltSocketData :public CMySocket
{
public:
	CCltSocketData(UINT Port);
	~CCltSocketData(void);

	virtual void SetSocketInfo(CString name = L"");
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	int PreGetFile(MYMSG msg);
	int GetFileEnd();

	FILE* m_pFile;
	ULONGLONG m_nCommSize;
	MYUPLOADFILE m_msg;
};

