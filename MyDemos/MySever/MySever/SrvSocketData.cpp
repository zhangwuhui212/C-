#include "StdAfx.h"
#include "MySever.h"
#include "MySeverDlg.h"
#include "SrvSocketData.h"


CSrvSocketData::CSrvSocketData(UINT Port)
{
	m_Port = Port;
	m_pFile = NULL;
	m_hTask = NULL;
	m_nCommSize = 0;
	memset(&m_msg,0,sizeof(m_msg));
}


CSrvSocketData::~CSrvSocketData(void)
{
	if(m_hTask)
	{
		TerminateThread(m_hTask, 0);
		CloseHandle(m_hTask);
		m_hTask = NULL;
	}

	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

void CSrvSocketData::SetSocketInfo(CString name/* = L""*/)
{
	SOCKADDR_IN sa;
	int len = sizeof(sa);
	GetPeerName((SOCKADDR*)&sa, &len);
	m_node.addr = ntohl(sa.sin_addr.S_un.S_addr);
	m_node.port = ntohs(sa.sin_port);
}

void CSrvSocketData::OnReceive(int nErrorCode)
{
	char buf[4096] = {0};
	UINT len = Receive(buf, 4096);
	if (len != SOCKET_ERROR)
	{
		if (m_pFile)
		{
			fwrite(buf,len,1,m_pFile);
			m_nCommSize += len;
			((CMySeverDlg *)(theApp.m_pMainWnd))->OnProgress(this);
		}
	}

	CSocket::OnReceive(nErrorCode);
}

void CSrvSocketData::OnClose(int nErrorCode)
{
	((CMySeverDlg *)(theApp.m_pMainWnd))->OnSrvCloseCommand(this, nErrorCode);
	CSocket::OnClose(nErrorCode);
}

int CSrvSocketData::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return  FALSE;
	}

	int nLeft, nWritten;
	PBYTE pBuf = (PBYTE)lpBuf;
	nLeft = nBufLen;

	while (nLeft > 0)
	{
		nWritten = SendChunk(pBuf, nLeft);
		if (nWritten == SOCKET_ERROR)
			return nWritten;

		nLeft -= nWritten;
		pBuf += nWritten;
	}
	return nBufLen - nLeft;
}

int CSrvSocketData::SendChunk(const void* lpBuf, int nBufLen)
{
	int nResult;
	while ((nResult = CAsyncSocket::Send(lpBuf, nBufLen, 0)) == SOCKET_ERROR)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			Sleep(10);
		}
		else
			return SOCKET_ERROR;
	}
	return nResult;
}

int CSrvSocketData::PrePutFile(CStringA strFile)
{
	ASSERT(m_pFile == NULL);
	ASSERT(m_hTask == NULL);
	m_nCommSize = 0;
	m_msg.type = My_UploadFile_Type_TXT;
	strcpy_s(m_msg.name , strFile);
	fopen_s(&m_pFile, m_msg.name, "rb");
	if (m_pFile)
	{
		fseek(m_pFile, 0L, SEEK_END);
		m_msg.len = ftell(m_pFile);  
		fseek(m_pFile, 0L, SEEK_SET);
	}else
	{
		return -1;
	}

	return 0;
}

int CSrvSocketData::StartPutFile()
{
	//ASSERT(m_hTask==NULL);
	TRACE("push......");
	m_hTask = ::CreateThread(NULL,0,PutFileThread,this,0,NULL);
	return 0;
}

DWORD WINAPI CSrvSocketData::PutFileThread(LPVOID lpvoid)
{
	CSrvSocketData* pThis = (CSrvSocketData*)lpvoid;
	int ret = 0;
	while(TRUE)
	{
		char buf[1024] = {0};

		int len = fread(buf, 1, 1024, pThis->m_pFile);
		if (len==0)
		{
			ret = -1;
			break;
		}
		len = pThis->Send(buf, len);
		if(len == SOCKET_ERROR)
		{
			ret = -2;
			break;
		}
		pThis->m_nCommSize += len;
		((CMySeverDlg *)(theApp.m_pMainWnd))->OnProgress(pThis);
		//Sleep(10);
	}
	fclose(pThis->m_pFile);
	pThis->m_pFile = NULL;

	((CMySeverDlg *)(theApp.m_pMainWnd))->PushFileEnd(ret,pThis);
	return ret;
}

