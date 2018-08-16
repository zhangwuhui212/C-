
// EvacDLSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EvacDLS.h"
#include "EvacDLSDlg.h"
#include "afxdialogex.h"
#include "TftpComm.h"
#include "SrvListen.h"
#include "SrvSocket.h"
#include "TftpSrvListen.h"
#include "TftpSrvSocket.h"
#include "TftpCltSocket.h"
#include "FindSocket.h"


const char heart_msg[64] = "is server heart msg......";


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEvacDLSDlg dialog




CEvacDLSDlg::CEvacDLSDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEvacDLSDlg::IDD, pParent)
	, m_port(0)
	, m_ip(0)
{
	m_ip = htonl(inet_addr("192.168.16.21"));
	m_port = 5000;
	m_pSctListen = NULL;
	m_pClientList.RemoveAll();
	m_timer = 0;
	m_pTftpSrvListen = NULL;
	m_pSrvList.RemoveAll();
	m_pTftpCltSocket  = NULL;
	m_pFindBroadcast = NULL;
	m_pCltSocket =  NULL;
	memset(&m_addr, 0 , sizeof(m_addr));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEvacDLSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_listBox_ctrl);
	DDX_Control(pDX, IDC_LIST3, m_clientList_ctrl);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IP_ctrl);
	DDX_Control(pDX, IDC_EDIT1, m_port_edit);
	DDX_Text(pDX, IDC_EDIT1, m_port);
	DDV_MinMaxInt(pDX, m_port, 0, 9999);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
}

BEGIN_MESSAGE_MAP(CEvacDLSDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CEvacDLSDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CEvacDLSDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CEvacDLSDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CEvacDLSDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CEvacDLSDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CEvacDLSDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CEvacDLSDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CEvacDLSDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CEvacDLSDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CEvacDLSDlg::OnBnClickedButton10)
END_MESSAGE_MAP()


// CEvacDLSDlg message handlers

BOOL CEvacDLSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//m_IP_ctrl.SetAddress(192,168,16,21);
	//m_port_edit.SetWindowTextW(L"5000");
	LONG lStyle;
	lStyle = GetWindowLong(m_clientList_ctrl.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_clientList_ctrl.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = m_clientList_ctrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	m_clientList_ctrl.SetExtendedStyle(dwStyle); //设置扩展风格

	m_clientList_ctrl.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );//插入列
	m_clientList_ctrl.InsertColumn( 1, L"Ip", LVCFMT_LEFT, 100 );
	m_clientList_ctrl.InsertColumn( 2, L"State", LVCFMT_LEFT, 60 );


	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(69);
	m_addr.sin_addr.s_addr = inet_addr("192.168.16.21");

	m_timer = SetTimer(0x110,10000,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEvacDLSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEvacDLSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEvacDLSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEvacDLSDlg::AddClient(CSrvSocket * pClt)
{
	CString str;
	SOCKADDR_IN sa;
	int len = sizeof(sa);
	pClt->GetPeerName((SOCKADDR*)&sa, &len);
	pClt->m_node.state = 1;
	pClt->m_node.addr = sa.sin_addr.S_un.S_addr;
	pClt->m_node.port = htons(sa.sin_port);

	str.Format(L"%s:[%d]",CString(inet_ntoa(*(in_addr*)&(pClt->m_node.addr))),pClt->m_node.port);

	int nRow = m_clientList_ctrl.InsertItem(0, L"0");
	m_clientList_ctrl.SetItemText(nRow, 1, str);
	m_clientList_ctrl.SetItemText(nRow, 2, CString(L"已连接"));
}

POSITION CEvacDLSDlg::FindClient(CString strIPP)
{
	CString str;
	POSITION pz = m_pClientList.GetHeadPosition();

	while(pz != NULL)
	{
		CSrvSocket * pClt = m_pClientList.GetAt(pz);

		str.Format(L"%s:[%d]",CString(inet_ntoa(*(in_addr*)&(pClt->m_node.addr))),pClt->m_node.port);
		if (str == strIPP)
		{
			return pz;
		}
		m_pClientList.GetNext(pz);
	}
	return pz;
}

void CEvacDLSDlg::OnAcceptClient(CSrvListen * pListen,int nErrorCode)
{
	CSrvSocket* pClt = new CSrvSocket(this);	
	m_pSctListen->Accept(*pClt);
	m_pClientList.AddTail(pClt);
	AddClient(pClt);
	m_listBox_ctrl.AddString(L"one client connnectted .......");
}

void CEvacDLSDlg::OnReceiveData(CSrvSocket * pClt,char * data,int len)
{
	pClt->m_node.tc = GetTickCount();
	m_listBox_ctrl.AddString(CString(data));
}

void CEvacDLSDlg::OnClientClose(CSrvSocket * pClt,int nErrorCode)
{
	POSITION pos = m_pClientList.Find(pClt,0);
	if(pos!=NULL)
	{
		m_pClientList.RemoveAt(pos);
		pClt->Close();
		pClt->ShutDown(1);
		delete pClt;
		pClt = NULL;
	}

	if (pClt==m_pCltSocket)
	{
		m_pCltSocket->Close();
		m_pCltSocket->ShutDown(1);
		delete m_pCltSocket;
		m_pCltSocket = NULL;
	}
	m_listBox_ctrl.AddString(L"one client close .......");
}

void CEvacDLSDlg::OnReceiveTftpData(CTftpSrvListen * pClt,sockaddr_in sour_addr,char * data,int len)
{
	CString str;
	int ret = 0;
	CTftpSrvSocket * loader = NULL;
	switch(data[1])
	{
	case TFTP_RRQ:
		{
			loader = new CTftpSrvSocket(this);
			loader->Create(0,SOCK_DGRAM);
			m_pSrvList.Add(loader);
			ret = loader->OnGetFile(sour_addr, data, len);
			if ( 0 != ret)
			{
				loader->Close();
				delete loader;
				loader = NULL;
			}
			else
			{
				str.Format(L"Get from %s[%d] GET msg.",CString(inet_ntoa(sour_addr.sin_addr)),sour_addr.sin_port);
				m_listBox_ctrl.AddString(str);
			}
		}
		
		break;
	case TFTP_WRQ:
		{
			loader = new CTftpSrvSocket(this);
			loader->Create(0,SOCK_DGRAM);
			m_pSrvList.Add(loader);
			ret = loader->OnPutFile(sour_addr, data, len);
			if ( 0 != ret)
			{
				loader->Close();
				delete loader;
				loader = NULL;
			}
			else
			{
				str.Format(L"Get from %s[%d] PUT msg.",CString(inet_ntoa(sour_addr.sin_addr)),sour_addr.sin_port);
				m_listBox_ctrl.AddString(str);
			}
		}
		break;
	case TFTP_ERROR:
		if(strnicmp(data + 2 ,"ABORT", 4)==0 && (ret = FindSrvExist(sour_addr)) != -1)
		{
			loader  = m_pSrvList.GetAt(ret);
			loader->OnAbort();
			loader->Close();
			delete loader;
			loader = NULL;
		}
		break;
	}
}


void CEvacDLSDlg::OnTftpSrvClose(CTftpSrvSocket * pClt,int nErrorCode)
{
	CString str;
	int ret = 0;
	CTftpSrvSocket * loader = NULL;
	if((ret = FindSrvExist(pClt->m_addr)) != -1)
	{
		loader  = m_pSrvList.GetAt(ret);
		if (loader->m_tftpInfo.opcode == TFTP_RRQ)
		{
			str.Format(L"Put File %s[%d] To %s[%d] End.",
				CString(loader->m_tftpInfo.filename),loader->m_tftpInfo.fileoffset,
				CString(inet_ntoa(loader->m_addr.sin_addr)),loader->m_addr.sin_port);
			m_listBox_ctrl.AddString(str);
		}
		if (loader->m_tftpInfo.opcode == TFTP_WRQ)
		{
			str.Format(L"Get File %s[%d] From %s[%d] End.",
				CString(loader->m_tftpInfo.filename),loader->m_tftpInfo.fileoffset,
				CString(inet_ntoa(loader->m_addr.sin_addr)),loader->m_addr.sin_port);
			m_listBox_ctrl.AddString(str);
		}
		m_pSrvList.RemoveAt(ret);
		loader->Close();
		delete loader;
		loader = NULL;
	}
}

void CEvacDLSDlg::OnTftpCltClose(CTftpCltSocket * pClt,int nErrorCode)
{
	CString str;
	

	if (m_pTftpCltSocket->m_tftpInfo.opcode == TFTP_RRQ)
	{
		str.Format(L"Get File %s[%d] from TFTP:59 End.",CString(m_pTftpCltSocket->m_tftpInfo.filename),m_pTftpCltSocket->m_tftpInfo.fileoffset);
		m_listBox_ctrl.AddString(str);
	}
	if (m_pTftpCltSocket->m_tftpInfo.opcode == TFTP_WRQ)
	{
		str.Format(L"Put File %s[%d] to TFTP:59 End.",CString(m_pTftpCltSocket->m_tftpInfo.filename),m_pTftpCltSocket->m_tftpInfo.fileoffset);
		m_listBox_ctrl.AddString(str);
	}
	m_pTftpCltSocket->Close();
	delete m_pTftpCltSocket;
	m_pTftpCltSocket = NULL;

	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
}

void CEvacDLSDlg::OnReceiveFindData(CFindSocket * pClt,sockaddr_in addr,char * data,int len)
{
	int pos = 0;
	char sFindBroadcast[64] = "";
	char sTHWid[16] = "EVAC";
	char sTTName[9] = "FMS";
	char sTPz[9]    = "L";
	char sLName[21] = "FMS LEFT";
	char sManCode[4] = "HNP";
	int addr_len = sizeof(sockaddr_in);
	if (m_pFindBroadcast->m_bIsServer)
	{
		m_listBox_ctrl.AddString(L"Find Sever recv broadcast.");
		if (data[0] == FIND_ANS)
		{
			m_pFindBroadcast->AddCLient(addr.sin_addr.s_addr);
			m_listBox_ctrl.AddString(L"One Client is find......");
		}
	}
	else
	{
		m_listBox_ctrl.AddString(L"Find Client recv broadcast.");
		if (data[0] == FIND_REQ)
		{
			pos = 0;
			sFindBroadcast[pos]  = FIND_ANS;
			pos++;
			strcpy(sFindBroadcast + pos,sTHWid);
			pos += strlen(sTHWid);
			sFindBroadcast[pos]  = 0x00;
			pos++;

			strcpy(sFindBroadcast + pos,sTTName);
			pos += strlen(sTTName);
			sFindBroadcast[pos]  = 0x00;
			pos++;

			strcpy(sFindBroadcast + pos,sTPz);
			pos += strlen(sTPz);
			sFindBroadcast[pos]  = 0x00;
			pos++;

			strcpy(sFindBroadcast + pos,sLName);
			pos += strlen(sLName);
			sFindBroadcast[pos]  = 0x00;
			pos++;

			strcpy(sFindBroadcast + pos,sManCode);
			pos += strlen(sManCode);
			sFindBroadcast[pos]  = 0x00;
			pos++;

			sFindBroadcast[pos]  = 0x10;
			pos++;
			m_listBox_ctrl.AddString(L"Server is findding......");
			m_pFindBroadcast->SendTo(sFindBroadcast,pos,(sockaddr *)&addr,addr_len);
		}
	}
}

int CEvacDLSDlg::FindSrvExist(sockaddr_in addr)
{
	CTftpSrvSocket *  loader = NULL;
	for(int i = 0;i<m_pSrvList.GetCount();i++)
	{
		loader  = m_pSrvList.GetAt(i);
		if (loader->m_addr.sin_addr.s_addr==addr.sin_addr.s_addr &&
			loader->m_addr.sin_port==addr.sin_port)
		{
			return i;
		}
	}
	return -1;
}


void CEvacDLSDlg::OnDestroy()
{
	if (m_timer != 0)
	{
		KillTimer(m_timer);
		m_timer = 0;
	}

	if (m_pCltSocket!=NULL)
	{
		m_pCltSocket->Close();
		delete m_pCltSocket;
		m_pCltSocket = NULL;
	}

	if (m_pFindBroadcast!=NULL)
	{
		m_pFindBroadcast->Close();
		delete m_pFindBroadcast;
		m_pFindBroadcast = NULL;
	}

	if (m_pTftpCltSocket!=NULL)
	{
		m_pTftpCltSocket->Close();
		m_pTftpCltSocket->ShutDown(1);
		delete m_pTftpCltSocket;
		m_pTftpCltSocket = NULL;
	}

	while(m_pSrvList.GetCount()>0)
	{
		CTftpSrvSocket * loader = m_pSrvList.GetAt(0);
		TRACE("close 2...");
		m_pSrvList.RemoveAt(0);
		delete loader;
		loader = NULL;
	}

	if (m_pTftpSrvListen!=NULL)
	{
		m_pTftpSrvListen->Close();
		m_pTftpSrvListen->ShutDown(1);
		delete m_pTftpSrvListen;
		m_pTftpSrvListen = NULL;
	}

	if(m_pSctListen != NULL)
	{
		m_pSctListen->Close();
		m_pSctListen->ShutDown(1);
		delete m_pSctListen;
		m_pSctListen = NULL;
	}

	while(m_pClientList.GetCount()>0)
	{
		CSrvSocket * pClt = m_pClientList.GetHead();
		pClt->Close();
		pClt->ShutDown(1);
		delete pClt;
		pClt = NULL;
		m_pClientList.RemoveHead();
	}
	CDialogEx::OnDestroy();
}


void CEvacDLSDlg::OnBnClickedButton1()
{
	//start listen
	UpdateData();
	m_pSctListen = new CSrvListen(this);
	m_pSctListen->Create(m_port);
	m_pSctListen->Listen();
	m_listBox_ctrl.AddString(L"Start Listen .......");

	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
}


void CEvacDLSDlg::OnBnClickedButton2()
{
	CString str;
	char buf[64] = "send file start......";
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	for(int i = 0; i < m_clientList_ctrl.GetItemCount(); i++)
	{
		if( m_clientList_ctrl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED \
			|| m_clientList_ctrl.GetCheck(i))
		{
			str.Format(_T("第%d行的checkbox为选中状态"), i);
			m_listBox_ctrl.AddString(str);
			str = m_clientList_ctrl.GetItemText(i,1);
			POSITION pz = FindClient(str);
			if (pz!=NULL)
			{
				m_pClientList.GetAt(pz)->Send(buf,10);
			}
		}
	}

	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
}

void CEvacDLSDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString str;
	DWORD tt = GetTickCount();
	if (m_pSctListen!=NULL)
	{
		POSITION pz = m_pClientList.GetTailPosition();
			while(pz != NULL)
			{
				CSrvSocket * pClt = m_pClientList.GetAt(pz);
				if(pClt->m_node.state == 1 && (tt - pClt->m_node.tc) > 6000)
				{
					m_listBox_ctrl.AddString(L"one client is timeout!");
					m_pClientList.RemoveAt(pz);
					pClt->Close();
					pClt->ShutDown(1);
					delete pClt;
					pClt = NULL;
				}else
				{
					pClt->Send(heart_msg,strlen(heart_msg));
				}
				m_pClientList.GetPrev(pz);
			}
	}

	if (m_pTftpSrvListen!=NULL)
	{
		for(int i = (m_pSrvList.GetCount()-1);i > 0;i--)
		{
			CTftpSrvSocket * loader = m_pSrvList.GetAt(i);
			if((tt - loader->m_tftpInfo.tc) > 6000)
			{
				m_listBox_ctrl.AddString(L"one tftp client is timeout!");
				m_pSrvList.RemoveAt(i);
				loader->Close();
				delete loader;
				loader = NULL;
			}
		}
	}
	
	if (m_pTftpCltSocket!=NULL && (tt - m_pTftpCltSocket->m_tftpInfo.tc) > 6000)
	{
		m_listBox_ctrl.AddString(L"tftp clent is timeout!");
		m_pTftpCltSocket->Close();
		delete m_pTftpCltSocket;
		m_pTftpCltSocket = NULL;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CEvacDLSDlg::OnBnClickedButton3()
{
	m_pTftpSrvListen =  new CTftpSrvListen(this);
	m_pTftpSrvListen->Create(69,SOCK_DGRAM);
	m_listBox_ctrl.AddString(L"Start Tftp Server...");
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
}

void CEvacDLSDlg::OnBnClickedButton4()
{
	CString str;
	char filename[] = "d:\\tt.txt";
	m_pTftpCltSocket = new CTftpCltSocket(this);
	m_pTftpCltSocket->Create(6000,SOCK_DGRAM);
	if (m_pTftpCltSocket->PutFile(m_addr , filename , strlen(filename))!= 0)
	{
		m_pTftpCltSocket->Close();
		delete m_pTftpCltSocket;
		m_pTftpCltSocket = NULL;
	}
	else
	{
		str.Format(L"Put File %s to TFTP:59.",CString(filename));
		m_listBox_ctrl.AddString(str);

		GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	}
}


void CEvacDLSDlg::OnBnClickedButton5()
{
	CString str;
	char filename[] = "tt.txt";
	m_pTftpCltSocket = new CTftpCltSocket(this);
	m_pTftpCltSocket->Create(6000,SOCK_DGRAM);
	if (m_pTftpCltSocket->GetFile(m_addr , filename , strlen(filename))!= 0)
	{
		m_pTftpCltSocket->Close();
		delete m_pTftpCltSocket;
		m_pTftpCltSocket = NULL;
	}
	else
	{
		str.Format(L"Get File %s from TFTP:59.",CString(filename));
		m_listBox_ctrl.AddString(str);

		GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	}
}


void CEvacDLSDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
}


void CEvacDLSDlg::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
}


void CEvacDLSDlg::OnBnClickedButton8()
{
	BOOL bBroadcast = TRUE;
	if (m_pFindBroadcast==NULL)
	{
		m_pFindBroadcast = new CFindSocket(this);
		m_pFindBroadcast->Create(FIND_PORT,SOCK_DGRAM,L"192.168.16.21");
		m_pFindBroadcast->SetSockOpt(SO_BROADCAST,(char*)&bBroadcast,sizeof(bBroadcast));
		m_listBox_ctrl.AddString(L"Find Sever start......");
	}
	m_pFindBroadcast->SendFindBroadcast();
	m_listBox_ctrl.AddString(L"Find Sever send broadcast.");
}


void CEvacDLSDlg::OnBnClickedButton9()
{
	if (m_pFindBroadcast==NULL)
	{
		m_pFindBroadcast = new CFindSocket(this);
		m_pFindBroadcast->Create(FIND_PORT,SOCK_DGRAM);
		m_listBox_ctrl.AddString(L"Find Client start......");
	}	
}


void CEvacDLSDlg::OnBnClickedButton10()
{
	UpdateData();
	if (m_pCltSocket==NULL)
	{
		m_pCltSocket = new CSrvSocket(this);
		m_pCltSocket->Create();
		m_pCltSocket->Connect(L"192.168.16.21",m_port);
	}
}
