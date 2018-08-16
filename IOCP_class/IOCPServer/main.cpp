#include <vector>
#include <iostream>
#include "IOCP.h"

using namespace std;

class CMyServer : public CIOCPServer
{
public:
    void OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
    {
        printf("one client is connectted(%d): %s\n",
               GetCurrentConnection(), ::inet_ntoa(pContext->addrRemote.sin_addr));
        printf("recv data, size: %d btye\n", pBuffer->nLen);

        SendText(pContext, pBuffer->buff, pBuffer->nLen);
    }

    void OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
    {
        printf("one client closed...\n");
    }

    void OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError)
    {
        printf("one client error: %d\n", nError);
    }

    void OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
    {
        printf("recv data, size: %d btye, %s\n", pBuffer->nLen,pBuffer->buff);
        SendText(pContext, pBuffer->buff, pBuffer->nLen);
    }

    void OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
    {
        printf("send data ok, size: %d byte\n ", pBuffer->nLen);
    }
};

int main()
{
    CMyServer *pServer = new CMyServer;
    // 开启服务
    if(pServer->Start())
    {
        printf("server is start...\n");
    }
    else
    {
        printf("init server error!\n");
        return 1;
    }
    // 创建事件对象，让ServerShutdown程序能够关闭自己
    HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, L"ShutdownEvent");
    ::WaitForSingleObject(hEvent, INFINITE);
    ::CloseHandle(hEvent);
    // 关闭服务
    pServer->Shutdown();
    delete pServer;
    printf("server is closed.\n ");
    return 0;
}
