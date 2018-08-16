// TftpCltSocket.cpp : implementation file
//

#include "stdafx.h"
#include "EvacDLS.h"
#include "TftpCltSocket.h"
#include "EvacDLSDlg.h"


// CTftpCltSocket

CTftpCltSocket::CTftpCltSocket(CEvacDLSDlg * pMainDlg)
{
	m_pMainDlg = pMainDlg;
	memset(&m_addr, 0, sizeof(m_addr));
	memset(&m_tftpInfo, 0, sizeof(m_tftpInfo));
	m_pFile = NULL;
	m_tftpInfo.tc = GetTickCount();
}

CTftpCltSocket::~CTftpCltSocket()
{
	if (m_pFile!=NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}


// CTftpCltSocket member functions


void CTftpCltSocket::OnReceive(int nErrorCode)
{
	char data[1024] ={0};
	int len  = 0 , addrlen = 0;
	char buf[1024] = {0};
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addrlen = sizeof(sockaddr);
	len = ReceiveFrom(buf,1024,(SOCKADDR *)(&addr), &addrlen);
	m_tftpInfo.tc = GetTickCount();
	if(SOCKET_ERROR != len && m_pFile!=NULL)
	{
		if (m_tftpInfo.state == TFTP_WSTAT_FIRSTACK)
		{
			m_tftpInfo.state = TFTP_WSTAT_NEXTACK;
			m_addr = addr;
		}
		switch(buf[1])
		{
		case TFTP_DATA:
			if(m_tftpInfo.opcode == TFTP_RRQ)
			{
				m_tftpInfo.lastblocknum = MAKEWORD(buf[3],buf[2]);
				if (m_tftpInfo.lastblocknum == m_tftpInfo.blocknum)
				{
					memset(m_tftpInfo.msg, 0 , sizeof(m_tftpInfo.msg));
					m_tftpInfo.msglen = makeack( m_tftpInfo.msg ,m_tftpInfo.blocknum);
					SendTo(m_tftpInfo.msg , m_tftpInfo.msglen ,(sockaddr *)&m_addr,sizeof(m_addr));
					m_tftpInfo.blocknum++;
					if(m_tftpInfo.blocknum > 65535)
						m_tftpInfo.blocknum = 0;
					if (len <= TFTP_NOTEND_DATALEN)
					{
						fwrite(&buf[4], 1, len - 4, m_pFile);
						m_tftpInfo.fileoffset += len - 4;
						//on progress
						if (len < TFTP_NOTEND_DATALEN)
						{
							//on read file end
							m_pMainDlg->OnTftpCltClose(this,0);
						}
					}
				}else
				{
					//error
				}
			}
			else
			{
				//error
			}
			break;
		case TFTP_ACK:
			if(m_tftpInfo.opcode == TFTP_WRQ)
			{
				m_tftpInfo.lastblocknum= MAKEWORD(buf[3],buf[2]);
				if (m_tftpInfo.lastblocknum == m_tftpInfo.blocknum)
				{
					switch(m_tftpInfo.state)
					{
					case TFTP_WSTAT_FIRSTACK:
					case TFTP_WSTAT_NEXTACK:
						m_tftpInfo.rlen = fread( data , 1, 512 ,m_pFile);
						m_tftpInfo.fileoffset += m_tftpInfo.rlen;
						//m_tftpInfo.state = TFTP_WSTAT_NEXTACK;
						if( m_tftpInfo.rlen < 512 && feof(m_pFile))
						{
							m_tftpInfo.state = TFTP_WSTAT_LASTACK;
						}
						else
						{
							if(ferror(m_pFile))
							{
								memset(m_tftpInfo.msg, 0 , sizeof(m_tftpInfo.msg));
								m_tftpInfo.msglen = makeerr(m_tftpInfo.msg, Not_defined, "");
								SendTo(m_tftpInfo.msg , m_tftpInfo.msglen ,(sockaddr *)&m_addr,sizeof(m_addr));
								return;
							}
						}
						m_tftpInfo.blocknum++;
						if(m_tftpInfo.blocknum > 65535)
							m_tftpInfo.blocknum = 0;
						memset(m_tftpInfo.msg, 0 , sizeof(m_tftpInfo.msg));
						m_tftpInfo.msglen = makedata( m_tftpInfo.msg, m_tftpInfo.blocknum , data, m_tftpInfo.rlen);
						SendTo(m_tftpInfo.msg , m_tftpInfo.msglen ,(sockaddr *)&m_addr,sizeof(m_addr));
						break;
					case TFTP_WSTAT_LASTACK:
						//on send file end
						m_pMainDlg->OnTftpCltClose(this,0);
						break;
					}
				}else
				{
					//error
				}
			}
			else
			{
				//error
			}
			break;
		case TFTP_ERROR:
			//on w/r error stop w/r
			m_pMainDlg->OnTftpCltClose(this,1);
			break; 
		}
	}
	CSocket::OnReceive(nErrorCode);
}

int CTftpCltSocket::GetFile(sockaddr_in sour_addr,char * buf,int len)
{
	m_tftpInfo.opcode = TFTP_RRQ;
	m_addr = sour_addr;
	memset(m_tftpInfo.filename , 0, sizeof(m_tftpInfo.filename));
	strcpy(m_tftpInfo.filename ,buf);

	m_pFile = fopen(m_tftpInfo.filename , "wb+");
	m_tftpInfo.blocknum = 0;
	memset(m_tftpInfo.msg, 0 , sizeof(m_tftpInfo.msg));
	if (m_pFile!=NULL)
	{
		char filedir[1024];
		strcpy_s(filedir, m_tftpInfo.filename);
		::PathStripPathA(filedir);
		strcpy(m_tftpInfo.filename ,filedir);
		m_tftpInfo.state = TFTP_WSTAT_FIRSTACK;
		m_tftpInfo.msglen = makereq( m_tftpInfo.msg , m_tftpInfo.opcode , m_tftpInfo.filename ,"octet");
		SendTo( m_tftpInfo.msg, m_tftpInfo.msglen, (SOCKADDR *)(&m_addr),sizeof(m_addr));
		m_tftpInfo.blocknum++;
	}
	else
	{
		return -1;
	}
	return 0;
}

int CTftpCltSocket::PutFile(sockaddr_in sour_addr,char * buf,int len)
{
	char data[1024] ={0};
	m_tftpInfo.opcode = TFTP_WRQ;
	m_addr = sour_addr;
	memset(m_tftpInfo.filename , 0, sizeof(m_tftpInfo.filename));
	strcpy(m_tftpInfo.filename ,buf);

	m_pFile = fopen(m_tftpInfo.filename , "rb+");
	m_tftpInfo.blocknum = 0;
	memset(m_tftpInfo.msg, 0 , sizeof(m_tftpInfo.msg));
	if (m_pFile!=NULL)
	{
		char filedir[1024];
		strcpy_s(filedir, m_tftpInfo.filename);
		::PathStripPathA(filedir);
		strcpy(m_tftpInfo.filename ,filedir);
		m_tftpInfo.state = TFTP_WSTAT_FIRSTACK;
		m_tftpInfo.msglen = makereq( m_tftpInfo.msg , m_tftpInfo.opcode , m_tftpInfo.filename ,"octet");
		SendTo( m_tftpInfo.msg, m_tftpInfo.msglen, (SOCKADDR *)(&m_addr),sizeof(m_addr));
	}
	else
	{
		return -1;
	}
	return 0;
}

int CTftpCltSocket::OnAbort()
{
	int msglen = 0;
	char msg[1024]  ={0};
	msglen = makeerr( msg ,Not_defined, "");
	SendTo(msg , msglen ,(sockaddr *)&m_addr,sizeof(m_addr));
	return 0;
}
