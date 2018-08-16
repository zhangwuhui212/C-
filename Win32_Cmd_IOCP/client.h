#pragma once

#define RECEIVE_SIZE 1024
/****************************************************
//Client
function :
  save info of client
***************************************************/
class Client
{
public:
    Client();
    ~Client();
public:
    CCriticalSection	m_cs;

    BYTE				m_pRemainBuf[RECEIVE_SIZE];                 //数据缓存区
    BYTE				m_nDataLen;					//数据缓冲区中已有数据的长度
    BYTE				m_pReceBuf[RECEIVE_SIZE];                   //接受到的数据
    OVERLAPPED			m_OverLapped;

    WSABUF				m_DataBuf;
    SOCKADDR_IN			m_ClientAddress;

    //[!if NET_TYPE_TCP]
    SOCKET				m_hClientSock;							  //communication socket;
    //[!endif]

public:
    //***********将数据存入Client对应的缓冲区中*******************
    BOOL	SetData(int len);

    //***********从Client对应的缓冲区中取出数据*******************
    int		GetData(char * buff, int len);
};
