#include "StdAfx.h"
#include "IOCP_TCP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
*   Name
ThreadTcpServer
*	Type
public
*	Function
thread to monitor a socket and accept a link
*	Return Value
DWORD WINAPI
*	Parameters
void * lpParameter
*****************************************************************************/
DWORD WINAPI ThreadTcpServer(void * lpParameter)
{
    CompletionPort* pCpPort = (CompletionPort*)lpParameter;

    pCpPort->MonitorServerSocket();
    return 0;
}

/****************************************************************************
*   Name
CompletionRoutine
*	Type
public
*	Function
work thread of completionport
*	Return Value
DWORD WINAPI
*	Parameters
void * lpParameter translate a pointer of this
*****************************************************************************/
DWORD WINAPI  CompletionRoutine(void * lpParameter)
{
    CompletionPort* pCpPort = (CompletionPort*)lpParameter;

    pCpPort->MonitorIoCompletionPort();
    return 0;
}


/****************************************************************************
*   Name
CompletionPort
*	Type
public
*	Function
constructor
*	Return Value
null
*	Parameters
null
*****************************************************************************/
CompletionPort::CompletionPort()
{
    //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF| _CRTDBG_LEAK_CHECK_DF);
    m_hCOP = INVALID_HANDLE_VALUE;
    m_hListen = INVALID_HANDLE_VALUE;
    for (int i=0; i< MAXTHREAD_COUNT; i++)
    {
        m_hThreadArray[i] = INVALID_HANDLE_VALUE;
    }
    m_bQuitThread = FALSE;
    m_wsaEvent = NULL;
    m_ListenSocket = INVALID_SOCKET;
    m_ClientVector.clear();
    m_bIsQuit = FALSE;
    m_hQuitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_nThreadArray = 0;
}

/****************************************************************************
*   Name
~CompletionPort
*	Type
public
*	Function
do sth to release memory
*	Return Value
null
*	Parameters
null
*****************************************************************************/
CompletionPort::~CompletionPort()
{
    int i = 0;
    m_bIsQuit = TRUE;
    Sleep(10);
    if (INVALID_HANDLE_VALUE != m_hCOP)
    {
        m_bIsQuit = TRUE;
        //等待所有接受数据的线程结束
        WaitForMultipleObjects(m_nThreadArray, m_hThreadArray, TRUE, INFINITE);
        while (INVALID_HANDLE_VALUE != m_hThreadArray[i])
        {
            CloseHandle(m_hThreadArray[i]);
            m_hThreadArray[i-1] = INVALID_HANDLE_VALUE;
            i++;
        }
        //关闭完成端口
        CloseHandle(m_hCOP);
        m_hCOP=NULL;
    }

    // 关闭监听socket
    if (INVALID_SOCKET != m_ListenSocket)
    {
        closesocket(m_ListenSocket);
        m_ListenSocket = NULL;
    }

    // 关闭监听线程
    m_bQuitThread = TRUE;
    WaitForSingleObject(m_hListen, INFINITE);
    if (INVALID_HANDLE_VALUE != m_hListen)
    {
        CloseHandle(m_hListen);
        m_hListen = NULL;
    }

    WSACloseEvent(m_wsaEvent);
    m_wsaEvent = NULL;

    // 关闭客户端连接socket
    for( size_t i=0; i<m_ClientVector.size(); i++)
    {
        Client* pClient = m_ClientVector[i];
        if( pClient != NULL)
        {
            if (pClient->m_hClientSock != INVALID_SOCKET)
            {
                shutdown(pClient->m_hClientSock, 2);
                closesocket(pClient->m_hClientSock);
                pClient->m_hClientSock = INVALID_SOCKET;
            }
            delete pClient;
            pClient = NULL;
        }
    }
    CloseHandle(m_hQuitEvent);
    m_ClientVector.clear();
}


BOOL CompletionPort::Init(char * lpszHostAddress, UINT nHostPort)
{
    if (!InitWinsock())
    {
        return FALSE;
    }

    m_HostAddress.sin_family = AF_INET;

    m_HostAddress.sin_addr.s_addr = inet_addr((char *)lpszHostAddress);

    if (m_HostAddress.sin_addr.s_addr == INADDR_NONE)
    {
        LPHOSTENT lphost;
        lphost = gethostbyname((char *)lpszHostAddress);
        if (lphost != NULL)
            m_HostAddress.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
        else
        {
            WSASetLastError(WSAEINVAL);
            return FALSE;
        }
    }

    m_HostAddress.sin_port = htons((u_short)nHostPort);

    return true;
}

//************启动******************************
//	功能：	启动监听功能
//**********************************************
BOOL CompletionPort::Active()
{

//-------------------------------监听连接---------------------------------------
    //创建监听Socket
    m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == m_ListenSocket)
    {
        return FALSE;
    }

    int nResult = bind(m_ListenSocket, (PSOCKADDR)&m_HostAddress, sizeof(m_HostAddress));
    if (SOCKET_ERROR == nResult)
    {
        closesocket(m_ListenSocket);
        return FALSE;
    }

    nResult = listen(m_ListenSocket, 20);
    if (SOCKET_ERROR == nResult)
    {
        closesocket(m_ListenSocket);
        return FALSE;
    }

    m_wsaEvent = WSACreateEvent();
    if (NULL == m_wsaEvent)
    {
        return FALSE;
    }

    WSAEventSelect(m_ListenSocket, m_wsaEvent, FD_ACCEPT);

    //创建监听连接的线程
    m_hListen = CreateThread(NULL, 0, ThreadTcpServer, (LPVOID)this, 0, NULL);
    if (NULL == m_hListen)
    {
        return FALSE;
    }

//-------------------------------完成端口---------------------------------------
    //创建完成端口句柄
    m_hCOP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    if (NULL == m_hCOP)
    {
        return FALSE;
    }

    //创建处理数据的线程
    SYSTEM_INFO SysInfo;
    GetSystemInfo(&SysInfo);
    for (int i=0; i<(int)SysInfo.dwNumberOfProcessors && (i<MAXTHREAD_COUNT); i++)
    {
        m_hThreadArray[i] = CreateThread(NULL, 0, CompletionRoutine, (LPVOID)this, 0, NULL);
        if (NULL == m_hThreadArray[i])
        {
            while (i>0)
            {
                CloseHandle(m_hThreadArray[i-1]);
                m_hThreadArray[i-1] = INVALID_HANDLE_VALUE;
                i--;
            }
            return FALSE;
        }
        m_nThreadArray ++;
    }
    return TRUE;
}

//************从完成端口取数据******************
//	功能： 从完成端口取数据
//  返回： 字节数，如果ip未连接则返回0
//**********************************************
int CompletionPort::ReceiveData(char * buff, int len, char * ip)
{
    if (!buff || !ip)
    {
        return 0;
    }

    Client * pClient = GetClient(ip);

    if (!pClient)
    {
        return 0;
    }

    return pClient->GetData(buff, len);
}

//************发送数据***************************
//	功能：	向指定的客户端ip的数据
//	参数：  buff：指向发送数据的缓冲区
//			len:  要发送的数据的字节数
//			ip:	  客户端ip，如192.168.31.27
//  返回：	发送的字节数，错误则返回0
//**********************************************
int CompletionPort::SendData(const char * buff, int len, char * ip)
{
    if (!buff || !ip)
    {
        return 0;
    }

    int ret =0;

    //在取得客户端指针之后，完成端口线程会删除该客户端，
    //可能造成最null指针的解引用，所有这里使用结构化异常进行处理
    __try
    {
        Client * pClient = GetClient(ip);

        if (!pClient)
        {
            return 0;
        }

        DWORD result = 0;
        ret =send(pClient->m_hClientSock, buff, len, 0);
        //int ret = WriteFile((HANDLE)pClient->m_hClientSock, buff, len, &result, NULL);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        printf("客户端已被删除，发送失败");
    }
    return ret;
}

/****************************************************************************
*   Name
InitWinsock
*	Type
public
*	Function
initialize socket
*	Return Value
BOOL
*	Parameters
null
*****************************************************************************/
BOOL CompletionPort::InitWinsock()
{
    WSADATA wsd;
    int nResult = WSAStartup(MAKEWORD(2,2), &wsd);
    if (0 != nResult)
    {
        return FALSE;
    }
    return TRUE;
}

/****************************************************************************
*   Name
MonitorServerSocket
*	Type
public
*	Function
function to monitor a socket and accept a link
*	Return Value
null
*	Parameters
null
*****************************************************************************/
void CompletionPort::MonitorServerSocket()
{
    DWORD	dwWaitCode;
    WSANETWORKEVENTS wsaNetWorkEvents;
    while( true )
    {
        if( m_bQuitThread )					//判断监听线程退出条件是否满足
        {
            SetEvent(m_hQuitEvent);
            TRACE("Monitor Server Quit\n");
            return ;
        }

        dwWaitCode = WSAWaitForMultipleEvents(1, &m_wsaEvent, FALSE, 2000, FALSE);//等待网络事件
        if(dwWaitCode != WAIT_OBJECT_0)
            continue;

        if( SOCKET_ERROR == WSAEnumNetworkEvents(m_ListenSocket, m_wsaEvent,&wsaNetWorkEvents ))//枚举网络事件
        {
            int nErrorCode = WSAGetLastError();
            TRACE("WSAEnumNetworkEvents Error");
        }
        else
        {
            if( wsaNetWorkEvents.lNetworkEvents & FD_ACCEPT)
            {
                OnClientAccept();
            }
        }
    }
}

/****************************************************************************
*   Name
OnClientAccept
*	Type
public
*	Function
function to monitor a socket and accept a link
*	Return Value
null
*	Parameters
null
*****************************************************************************/
void CompletionPort::OnClientAccept()
{

    int nSockAddrLength = sizeof(SOCKADDR_IN);
    Client* pClntData = new Client();
    pClntData->m_hClientSock = accept(m_ListenSocket,
                        (struct sockaddr*)&(pClntData->m_ClientAddress),
                        &nSockAddrLength);

    if (INVALID_SOCKET == pClntData->m_hClientSock)
    {
        delete pClntData;
    }

    AddClient(pClntData);

    HANDLE hCompletePort = CreateIoCompletionPort((HANDLE)pClntData->m_hClientSock, m_hCOP,
        (DWORD)pClntData->m_hClientSock,0);
    if( hCompletePort != m_hCOP)
    {
        TRACE("Create Communication CompletionPort failure!\n");
    }

    ReadFile((HANDLE)pClntData->m_hClientSock, pClntData->m_pReceBuf, RECEIVE_SIZE, NULL,
        &pClntData->m_OverLapped);
}

/****************************************************************************
*   Name
MonitorIoCompletionPort
*	Type
public
*	Function
function to monitor completionport
*	Return Value
void
*	Parameters
null
*****************************************************************************/
void CompletionPort::MonitorIoCompletionPort()
{
    TRACE("Begin Monitor IoPort \n");
    BOOL  bSuc       = FALSE;
    DWORD dwNumBytes = 0;
    DWORD dwKey      = -1;
    DWORD dwSockId = 0;
    LPOVERLAPPED completedOverlapped;

    int nLen =0;
    while( true )
    {
        //判断监听线程退出条件是否满足
        if( m_bIsQuit)
        {
            TRACE("Monitor IoCompletionPort Quit\n");
            SetEvent(m_hQuitEvent);
            return;
        }
        //查询完全端口状态
        dwKey = -1;
        bSuc  = GetQueuedCompletionStatus( m_hCOP, &dwNumBytes, &dwKey, &completedOverlapped, 2000);
        if (bSuc==FALSE)
        {
            int nError = WSAGetLastError();

            if (nError == WSA_WAIT_TIMEOUT)
            {
                continue;
            }

            if (nError == ERROR_NETNAME_DELETED)
            {
                OnClientClose(dwKey);
                continue;
            }
            TRACE("GetQueuedCompletionStatus Error: %d\n", nError);
            continue;
        }


        if (dwKey>0)
        {
            dwSockId  = dwKey;
            nLen = dwNumBytes;
            if( nLen > 0)
            {
                int index = -1;
                Client* pClient = GetClient(dwSockId, index);
                if( pClient == NULL)
                    continue;
                try
                {
                    //Sleep(50);
                    if (nLen >1024)
                    {
                        nLen = 1024;
                        //CDebug::ShowErroMessage("接收到数据长度过长！");
                    }

                    pClient->SetData(nLen);
                    //ProcessData(pClient);

                    //向完全端口发送读指令，等待下一次数据
                    ZeroMemory(pClient->m_pReceBuf,RECEIVE_SIZE);

                    ReadFile((HANDLE)pClient->m_hClientSock,pClient->m_pReceBuf,RECEIVE_SIZE,NULL,&pClient->m_OverLapped);


                }
                catch(...)
                {
                    TRACE("catch an error");
                }
            }
            else
            {
                OnClientClose(dwSockId);
            }
        }
    }
}

/****************************************************************************
*   Name
GetClient
*	Type
public
*	Function
get a pointer to point class of  Client
*	Return Value
Client*
*	Parameters
DWORD dwSockId : socket ID
int& index: the position in the vector
*****************************************************************************/
Client* CompletionPort::GetClient(DWORD dwSockId, int& index)
{
    Client* pNewData = NULL;
    m_cs.lock();
    for (size_t i = 0; i < m_ClientVector.size(); i++)
    {
        if (m_ClientVector[i]->m_hClientSock == dwSockId)
        {
            index = i;
            pNewData = m_ClientVector[i];
            break;
        }
    }
    m_cs.unlock();
    return pNewData;
}

/****************************************************************************
*   Name
OnClientClose
*	Type
public
*	Function
close the socket that dont translate data
*	Return Value
BOOL
*	Parameters
DWORD dwSockId: socket ID
*****************************************************************************/
BOOL CompletionPort::OnClientClose(DWORD dwSockId)
{
    int index = -1;
    Client* pClient = GetClient(dwSockId, index);
    if ((NULL != pClient) && (-1 != index))
    {
        closesocket(pClient->m_hClientSock);
        DeleteClient(index);
        pClient = NULL;
    }
    return TRUE;
}

//----------------------------Client 管理-------------------------

//************增加客户端*********************
BOOL CompletionPort::AddClient(Client * const pClient)
{
    if (!pClient)
    {
        return FALSE;
    }
    m_cs.lock();
    m_ClientVector.push_back(pClient);
    m_cs.unlock();
    return TRUE;
}

//************删除客户端*********************
BOOL CompletionPort::DeleteClient(int index)
{
    m_cs.lock();
    delete m_ClientVector[index];
    m_ClientVector.erase(m_ClientVector.begin() + index);
    m_cs.unlock();
    return TRUE;
}

//************查询客户端*********************
Client * CompletionPort::GetClient(const char * ip)
{
    Client* pNewClient = NULL;
    ULONG nAddr = inet_addr(ip);
    m_cs.lock();
    for (size_t i = 0; i < m_ClientVector.size(); i++)
    {
        if (m_ClientVector[i]->m_ClientAddress.sin_addr.s_addr == nAddr)
        {
            pNewClient = m_ClientVector[i];
            break;
        }
    }
    m_cs.unlock();
    return pNewClient;
}
