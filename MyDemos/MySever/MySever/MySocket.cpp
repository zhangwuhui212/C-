#include "stdafx.h"
#include "MySocket.h"

CMySocket::CMySocket()
{
	m_TickCount = ::GetTickCount();
	memset(&m_node,0,sizeof(m_node));
}

CMySocket::~CMySocket()
{
}

// void CMySocket::ntohmsg(MYMSG & msg)
// {
// #define n2hl(a)	a = ntohl(a)
// 
//  	n2hl(msg.id);
//  	n2hl(msg.error);
// }
// 
// void CMySocket::htonmsg(MYMSG & msg)
// {
// #define h2nl(a)	a = htonl(a)
// 
// 	h2nl(msg.id);
// 	h2nl(msg.error);
// }

