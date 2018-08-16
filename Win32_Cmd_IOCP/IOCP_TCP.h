/************************************************************************/
//* 文件名称: IOCP_TCP.h
//* 文件标示:
//* 摘    要:采用完成端口技术处理网络数据
//*
//* 当前版本:1.0
//* 作    者:hxy
//* 完成日期:2013-11-16
//*
/************************************************************************/
#ifndef  _COMPLETIONPORT_H__
#define  _COMPLETIONPORT_H__

#include <winsock2.h>
#include <mswsock.h>
#include "afxmt.h"
#include <vector>
#include "Client.h"
using namespace std;
#pragma once

#define MAXTHREAD_COUNT 8
#define RECEIVE_SIZE  1024
#define PORT 502

class Client;
class CompletionPort
{
public:
    CompletionPort();
    ~CompletionPort();

    //************初始化****************************
    //	参数：  需要绑定的ip和port
    //	功能：	初始化socket库
    //			设置需要绑定的ip和port
    //**********************************************
    BOOL Init(char * lpszHostAddress, UINT nHostPort);

    //************启动******************************
    //	功能：	1,启动监听功能; 2,生成完成端口对象
    //**********************************************
    BOOL Active(void);

    //************停止******************************
    //	功能：	停止监听功能，
    //			释放Active中的申请的所有资源
    //**********************************************
    BOOL Close(void);

    //************接受数据**************************
    //	功能：	接受指定客户端ip的数据
    //	参数：  buff：指向接受数据的缓冲区
    //			len:  缓冲区的大小
    //			ip:	  客户端ip，如192.168.31.27
    //  说明：  buff的内存由调用者提供
    //**********************************************
    int ReceiveData(char * buff, int len, char * ip);

    //************发送数据***************************
    //	功能：	向指定的客户端ip的数据
    //	参数：  buff：指向发送数据的缓冲区
    //			len:  缓冲区的大小
    //			ip:	  客户端ip，如192.168.31.27
    //**********************************************
    int SendData(const char * buff, int len, char * ip);

//-------------------------内部使用--------------------------------
    //************监听*******************************
    //	功能：	监听客户端连接线程
    //***********************************************
    void MonitorServerSocket();

    //************监听客户端发送的数据***************
    //	功能：	监听客户端连接线程函数
    //***********************************************
    void MonitorIoCompletionPort();

    //************处理客户端连接*********************
    //	功能：	客户端连接处理函数
    //***********************************************
    void OnClientAccept();

    //************处理客户端关闭*********************
    //	功能：	客户端关闭处理函数
    //***********************************************
    BOOL OnClientClose(DWORD dwSockId);


private:
    //************初始化Socket库*********************
    //	功能：	初始化Socket库
    //***********************************************
    BOOL InitWinsock();


    void ProcessData(Client* pClient);

    Client* GetClient(DWORD dwSockId, int& index);

//----------------------------Client 管理-------------------------

    //************增加客户端*********************
    BOOL AddClient( Client * const pClient);

    //************删除客户端*********************
    BOOL DeleteClient( int index);

    //************查询客户端*********************
    Client * GetClient( const char * ip);



private:

    SOCKADDR_IN m_HostAddress;							//服务器端地址（ip，port，协议）等

    vector<Client*> m_ClientVector;						//客户端表
    rec_lock m_cs;								//客户端表同步

    BOOL			m_bIsQuit;							//退出标识
    HANDLE			m_hQuitEvent;						//退出事件句柄
    HANDLE			m_hCOP;								//完成端口句柄
    HANDLE			m_hThreadArray[MAXTHREAD_COUNT];	//接收数据线程句柄
    int				m_nThreadArray;						//接收数据线程数量

    HANDLE			m_hListen;							//监听连接线程句柄
    SOCKET			m_ListenSocket;						//监听socket

    BOOL			m_bQuitThread;						//监听线程退出标识
    WSAEVENT		m_wsaEvent;							//网络事件


};

#endif
