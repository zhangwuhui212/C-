#include "client.h"

/****************************************************************************
*   Name
Client
*	Type
public
*	Function
construction function
*	Return Value
null
*	Parameters
null
*****************************************************************************/
Client::Client()
{
    //[!if NET_TYPE_TCP]
    m_hClientSock = INVALID_SOCKET;
    //[!endif]

    m_nDataLen = 0;
    memset(m_pRemainBuf, 0, RECEIVE_SIZE);
    memset(m_pReceBuf,0,RECEIVE_SIZE);
    memset(&m_OverLapped, 0, sizeof(m_OverLapped));
}

/****************************************************************************
*   Name
~Client
*	Type
public
*	Function

*	Return Value
null
*	Parameters
null
*****************************************************************************/
Client::~Client()
{
    int a=0;
    a=3;
}


//*********************TCP接受数据************************
//**************缓冲区没有溢出，则将数据追加其后**********
BOOL Client::SetData( int len )
{
    if (len<0)
    {
        return FALSE;
    }

    if (len==0)
    {
        return TRUE;
    }

    m_cs.Lock();
    if (m_nDataLen+len>=RECEIVE_SIZE)
    {
        if (len<RECEIVE_SIZE)
        {
            memcpy_s(m_pRemainBuf, RECEIVE_SIZE, m_pReceBuf, len);
            m_nDataLen = len;
        }
        else
        {
            memcpy_s(m_pRemainBuf, RECEIVE_SIZE, m_pReceBuf, RECEIVE_SIZE);
            m_nDataLen = (BYTE)RECEIVE_SIZE;
        }
    }
    else
    {
        memcpy_s(m_pRemainBuf + m_nDataLen, RECEIVE_SIZE-m_nDataLen, m_pReceBuf, len);
        m_nDataLen += len;
    }

    m_cs.Unlock();

    return TRUE;
}


int Client::GetData( char * buff, int len )
{
    if (!buff)
    {
        return 0;
    }

    int result=0;
    m_cs.Lock();
    if (m_nDataLen <= len)
    {
        memcpy_s(buff, len, m_pRemainBuf, m_nDataLen);
        result = m_nDataLen;
        m_nDataLen = 0;
    }
    else
    {
        memcpy_s(buff, len, m_pRemainBuf, len);
        memmove_s(m_pRemainBuf, RECEIVE_SIZE, m_pRemainBuf + len, m_nDataLen-len);
        m_nDataLen -= len;
        result = len;
    }

    m_cs.Unlock();
    return result;
}

