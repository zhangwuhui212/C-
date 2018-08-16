#pragma once
#include "mysocket.h"
class CSrvSocketData :public CMySocket
{
public:
	CSrvSocketData(UINT Port);
	~CSrvSocketData(void);

	virtual void SetSocketInfo(CString name = L"");
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	int SendChunk(const void* lpBuf, int nBufLen);

	int PrePutFile(CStringA strFile);
	int StartPutFile();

	static DWORD WINAPI PutFileThread(LPVOID lpvoid);

	FILE * m_pFile;
	HANDLE m_hTask;
	UINT m_nCommSize;
	MYUPLOADFILE m_msg;
};

