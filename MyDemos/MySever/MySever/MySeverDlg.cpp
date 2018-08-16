
// MySeverDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MySever.h"
#include "MySeverDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMySeverDlg 对话框




CMySeverDlg::CMySeverDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMySeverDlg::IDD, pParent)
	, m_port(SERVER_PORT_COMMAND)
	, m_ip(0)
{
	m_pSocketCommand = NULL;
	m_pSocketData = NULL;
	m_PushArray.RemoveAll();
	m_pCurPushFile = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMySeverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_msgEdit);
	DDX_Control(pDX, IDC_EDIT2, m_fileEdit);
	DDX_Text(pDX, IDC_EDIT3, m_port);
	DDV_MinMaxInt(pDX, m_port, 1024, 65535);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
}

BEGIN_MESSAGE_MAP(CMySeverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMySeverDlg::OnBnClickedOk)
	ON_MESSAGE(WM_MY_PROGRESS, &CMySeverDlg::OnUpdateProgress)
	ON_BN_CLICKED(ID_DOWNLOAD_FILE, &CMySeverDlg::OnBnClickedDownloadFile)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CMySeverDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMySeverDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMySeverDlg 消息处理程序

BOOL CMySeverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	EnabelServerElement(FALSE);
	m_fileEdit.EnableFileBrowseButton();
	m_ip = ntohl(inet_addr(GetHostIp()));
	m_port = SERVER_PORT_COMMAND;
	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMySeverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMySeverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMySeverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMySeverDlg::EnabelServerElement(BOOL btrue)
{
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(!btrue);
	GetDlgItem(IDC_EDIT3)->EnableWindow(!btrue);

	GetDlgItem(IDC_BUTTON1)->EnableWindow(!btrue);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(btrue);
	GetDlgItem(IDOK)->EnableWindow(btrue);
	GetDlgItem(ID_DOWNLOAD_FILE)->EnableWindow(btrue);

	GetDlgItem(IDC_EDIT1)->EnableWindow(btrue);
	GetDlgItem(IDC_EDIT2)->EnableWindow(btrue);
}

char * CMySeverDlg::GetHostIp()
{
	char HostName[100];
	gethostname(HostName, sizeof(HostName));
	hostent* hn = gethostbyname(HostName);
	return inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);
}

int CMySeverDlg::StartServer()
{
	m_pSocketCommand = new CSrvListen(SERVER_PORT_COMMAND);
	m_pSocketCommand->Create(SERVER_PORT_COMMAND);
	m_pSocketCommand->Listen(1);

	m_pSocketData = new CSrvListen(SERVER_PORT_DATA);
	m_pSocketData->Create(SERVER_PORT_DATA);
	m_pSocketData->Listen(1);

	PrintMsg(L"server start listen ......");
	return 0;
}

int CMySeverDlg::StopServer()
{
	m_PushArray.RemoveAll();
	if (m_pCurPushFile)
	{
		delete m_pCurPushFile;
		m_pCurPushFile = NULL;
	}

	while(m_ClientData.GetCount() > 0)
	{
		CMySocket * p = m_ClientData.GetAt(0);
		p->ShutDown();
		p->Close();
		delete p;
		m_ClientData.RemoveAt(0);
	}

	while(m_ClientCmd.GetCount() > 0)
	{
		CMySocket * p = m_ClientCmd.GetAt(0);
		p->ShutDown();
		p->Close();
		delete p;
		m_ClientCmd.RemoveAt(0);
	}

	if (m_pSocketData)
	{
		m_pSocketData->Close();
		delete m_pSocketData;
		m_pSocketData = NULL;
	}

	if (m_pSocketCommand)
	{
		m_pSocketCommand->Close();
		delete m_pSocketCommand;
		m_pSocketCommand = NULL;
	}

	return 0;
}

void  CMySeverDlg::OnAccept(int nErrorCode,UINT Port)
{
	if (Port==SERVER_PORT_COMMAND)
	{
		CSrvSocketCmd* Srv = new CSrvSocketCmd(SERVER_PORT_COMMAND);	
		m_pSocketCommand->Accept(*Srv);
		Srv->SetSocketInfo();
		Srv->m_TickCount = ::GetTickCount();
		m_ClientCmd.Add(Srv);
		OnClientConnect(Srv->m_node,SERVER_PORT_COMMAND);
	}
	else if (Port==SERVER_PORT_DATA)
	{
		CSrvSocketData* Srv = new CSrvSocketData(SERVER_PORT_DATA);	
		m_pSocketData->Accept(*Srv);
		Srv->SetSocketInfo();
		Srv->m_TickCount = ::GetTickCount();
		m_ClientData.Add(Srv);
		OnClientConnect(Srv->m_node,SERVER_PORT_DATA);

	}else
	{
		ASSERT(0);
	}
}

void CMySeverDlg::OnSrvCloseCommand(CMySocket * pSocket, int nErrorCode)
{
	for(int i=0; i<m_ClientCmd.GetCount(); i++)
	{
		if(m_ClientCmd[i] == pSocket)
		{
			m_ClientCmd.RemoveAt(i);
			pSocket->ShutDown();
			pSocket->Close();
			delete pSocket;
			PrintMsg(L"one client cmd close ......");
			break;
		}
	}

	for(int i=0; i<m_ClientData.GetCount(); i++)
	{
		if(m_ClientData[i] == pSocket)
		{
			m_ClientData.RemoveAt(i);
			pSocket->ShutDown();
			pSocket->Close();
			delete pSocket;
			PrintMsg(L"one client data close ......");
			break;
		}
	}
}

void CMySeverDlg::OnClose()
{	
	StopServer();

	CDialogEx::OnClose();
}

LRESULT CMySeverDlg::OnUpdateProgress(WPARAM wp ,LPARAM lp)
{
	CString str ;

	ULONGLONG len = wp;
	ULONGLONG mlen = lp;

	str.Format(L"push : %llu : %llu .",len,mlen);
	PrintMsg(str);
	return 0;
}

void CMySeverDlg::PrintMsg(CString str)
{
	int nIndex = m_list.AddString(str);
	m_list.SetCurSel(nIndex);
}

void CMySeverDlg::OnClientConnect(MYNODE node,UINT Port)
{
	union _addr{
		UINT addr;
		BYTE b[4];
	}u;
	u.addr = node.addr;

	CString str;
	str.Format(L"Client[%d] %u.%u.%u.%u[%d] connect......", Port, u.b[3], u.b[2], u.b[1], u.b[0],node.port);
	m_list.AddString(str);
}

void CMySeverDlg::OnBnClickedOk()
{
	CString str = L"";
	char buf[256] = {0};
	int si = GetWindowTextA(m_msgEdit.m_hWnd,buf,256);
	if (si > 0)
	{
		str = buf;
		str.Trim();
		if (str.GetLength()>1)
		{
			SrvSendMsg(str);
		}
		m_msgEdit.Clear();
	}
}

void CMySeverDlg::OnBnClickedDownloadFile()
{
	if(m_fileEdit.GetWindowTextLengthW()>0)
	{
		CString strFile = L"";
		m_fileEdit.GetWindowText(strFile);
		strFile.Trim();

		if(::PathFileExists(strFile))
		{
			CFile file ;
			file.Open(strFile, CFile::modeRead | CFile::typeBinary);
			if (file.m_hFile != CFile::hFileNull)
			{
				ULONGLONG len = file.GetLength();
				file.Close();

				for(int i=0; i<m_ClientCmd.GetCount(); i++)
				{
					CSrvSocketCmd * p = m_ClientCmd[i];
					int ret = m_PushArray.Add(CPushFile(CStringA(strFile), p->m_node.addr, p->m_node.port));
					m_PushArray[ret].m_TotalSize = len;
				}
				OnSchedulePutFile();
			}
		}
	}
}

void CMySeverDlg::OnSocketCommand(CMySocket * pSocket, MYMSG msg )
{
	switch(msg.id)
	{
	case My_Msg_Type_Msg:
		PrintMsg(CString(msg.buf1));
		break;
	case My_Msg_Type_Push_File:
		break;
	case My_Msg_Type_Push_File_Ack:
		OnPushFileAck( pSocket, msg);
		break;
	case My_Msg_Type_Push_File_Progress:
		break;
	case My_Msg_Type_Push_File_End:
		break;
	case My_Msg_Type_Push_File_End_Ack:
		OnPushFileEndAck(pSocket,msg);
		break;
	default:
		ASSERT(0);	//not arrived
		break;
	}
}

void CMySeverDlg::OnProgress(CMySocket * pSocket)
{
	ULONGLONG len = ((CSrvSocketData*)pSocket)->m_msg.len;
	ULONGLONG mlen = ((CSrvSocketData*)pSocket)->m_nCommSize;

	PostMessage(WM_MY_PROGRESS,(WPARAM)len,(LPARAM)mlen);
}

void CMySeverDlg::OnSchedulePutFile()
{
	if (m_pCurPushFile)
	{
		return;
	}
	for(int i=0; i<m_PushArray.GetCount(); i++)
	{
		if(m_PushArray[i].m_State == 0)
		{
			m_pCurPushFile = &m_PushArray[i];
			PushFile();
		}
	}
}

void CMySeverDlg::SrvSendMsg(CString str)
{
	MYMSG msg = {0};
	msg.id = My_Msg_Type_Msg;
	strcpy_s(msg.buf1,CStringA(str));
	for(int i=0; i<m_ClientCmd.GetCount(); i++)
	{
		m_ClientCmd[i]->Send(&msg,sizeof(msg));
	}
}

int CMySeverDlg::PushFile()
{
	for(int i=0; i<m_ClientCmd.GetCount(); i++)
	{
		if(m_ClientCmd[i]->m_node.addr == m_pCurPushFile->m_addr ||
			m_ClientCmd[i]->m_node.port == m_pCurPushFile->m_port)
		{
			MYMSG msg = {0};
			msg.id = My_Msg_Type_Push_File;
			msg.len = m_pCurPushFile->m_TotalSize;
			strcpy_s(msg.buf1,m_pCurPushFile->m_file);
			m_ClientCmd[i]->Send(&msg,sizeof(msg));
			PrintMsg(L"wait push file......");
			break;
		}
	}
	return 0;
}

void CMySeverDlg::OnPushFileAck(CMySocket * pSocket, MYMSG msg)
{
	for(int i=0; i<m_ClientData.GetCount(); i++)
	{
		if(m_ClientData[i]->m_node.addr == pSocket->m_node.addr)
		{
			PrintMsg(L"start push file......");
			m_ClientData[i]->PrePutFile(m_pCurPushFile->m_file);
			m_ClientData[i]->StartPutFile();
			break;
		}
	}
}

void CMySeverDlg::PushFileEnd(int error , CMySocket * pSocket)
{
	MYMSG msg = {0};
	msg.id = My_Msg_Type_Push_File_End;
	PrintMsg(L"end push file......");
	for(int i=0; i<m_ClientCmd.GetCount(); i++)
	{
		if(m_ClientCmd[i]->m_node.addr == pSocket->m_node.addr)
		{
			m_ClientCmd[i]->Send(&msg,sizeof(msg));
			break;
		}
	}
}

void CMySeverDlg::OnPushFileEndAck(CMySocket * pSocket, MYMSG msg)
{
	m_pCurPushFile->m_State = 2;
	m_pCurPushFile = NULL;
	OnSchedulePutFile();
}






void CMySeverDlg::OnBnClickedButton1()
{
	StartServer();
	EnabelServerElement(TRUE);
}


void CMySeverDlg::OnBnClickedButton2()
{
	StopServer();
	EnabelServerElement(FALSE);
}
