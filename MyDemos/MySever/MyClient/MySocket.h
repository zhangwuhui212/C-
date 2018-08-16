#pragma once

#include "MyComm.h"

class CMySocket : public CSocket
{
public:
	CMySocket();
	virtual ~CMySocket();
	virtual void SetSocketInfo(CString name = L"") = 0;

// 	void ntohmsg(MYMSG & msg);
// 	void htonmsg(MYMSG & msg);
	UINT m_Port;
	MYNODE m_node;
	UINT m_TickCount;
};


