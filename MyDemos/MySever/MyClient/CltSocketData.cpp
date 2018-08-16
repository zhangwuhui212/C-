#include "StdAfx.h"
#include "MyClient.h"
#include "MyClientDlg.h"
#include "CltSocketData.h"


CCltSocketData::CCltSocketData(UINT Port)
{
	m_Port = Port;
	m_pFile = NULL;
	m_nCommSize = 0;
	memset(&m_msg,0,sizeof(m_msg));
}

CCltSocketData::~CCltSocketData(void)
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

void CCltSocketData::SetSocketInfo(CString name/* = L""*/)
{
	SOCKADDR_IN sa;
	int len = sizeof(sa);
	GetSockName((SOCKADDR*)&sa, &len);
	m_node.addr = ntohl(sa.sin_addr.S_un.S_addr);
	m_node.port = ntohs(sa.sin_port);
}

void CCltSocketData::OnReceive(int nErrorCode)
{
	char buf[4096] = {0};
	UINT len = Receive(buf, 4096);
	if (len != SOCKET_ERROR)
	{
		if (m_pFile)
		{
			fwrite(buf,len,1,m_pFile);
			m_nCommSize += len;
			((CMyClientDlg *)(theApp.m_pMainWnd))->OnProgress(this);
		}
	}

	CSocket::OnReceive(nErrorCode);
}

void CCltSocketData::OnClose(int nErrorCode)
{
	((CMyClientDlg *)(theApp.m_pMainWnd))->OnSrvClosed(nErrorCode,m_Port);
	CSocket::OnClose(nErrorCode);
}

int CCltSocketData::PreGetFile(MYMSG msg)
{
	char dir[256] = {0};
	m_nCommSize = 0;
	memset(&m_msg,sizeof(m_msg),0);
	m_msg.type = My_UploadFile_Type_TXT;
	m_msg.len = msg.len;
	strcpy_s(m_msg.name , ::PathFindFileNameA(msg.buf1));
	fopen_s(&m_pFile,m_msg.name,"wb");
	if (!m_pFile)
	{
		return -1;
	}

	return 0;
}

int CCltSocketData::GetFileEnd()
{
	ULONGLONG nSendSize = m_msg.len;
	ASSERT(nSendSize >= m_nCommSize);
	if(nSendSize == m_nCommSize)
	{
		if(m_pFile)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}
		return 0;
	}
	ULONGLONG nLfet = nSendSize - m_nCommSize;
	ULONGLONG nTryCnt = 100;
	for(int i=0; i<nTryCnt; i++)
	{
		char buff[4096] = {0};
		int nResult = CAsyncSocket::Receive(buff, 4096, 0);
		if(nResult == SOCKET_ERROR)
		{
			if (GetLastError() == WSAEWOULDBLOCK)
			{
				Sleep(10);
			}
			else
			{
				if(m_pFile)
				{
					fclose(m_pFile);
					m_pFile = NULL;
					DeleteFile(CString(m_msg.name));
				}
				return -1;
			}
		}
		else
		{
			fwrite(buff, 1, nResult, m_pFile);
			m_nCommSize += nResult;
			((CMyClientDlg *)(theApp.m_pMainWnd))->OnProgress(this);

			ASSERT(nSendSize >= m_nCommSize);
			if(nSendSize == m_nCommSize)
			{
				if(m_pFile)
				{
					fclose(m_pFile);
					m_pFile = NULL;
				}
				return 0;
			}
			nLfet = nSendSize - m_nCommSize;
		}
	}
	
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		DeleteFile(CString(m_msg.name));
	}
	return 0;
}
