
// MyClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyClient.h"
#include "MyClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMyClientDlg �Ի���




CMyClientDlg::CMyClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyClientDlg::IDD, pParent)
	, m_ip(0)
	, m_port(SERVER_PORT_COMMAND)
{
	m_pSocketCommand = NULL;
	m_pSocketData = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_msgEdit);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
	DDV_MinMaxInt(pDX, m_port, 1024, 65535);
	DDX_Text(pDX, IDC_EDIT2, m_port);
}

BEGIN_MESSAGE_MAP(CMyClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMyClientDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_MY_PROGRESS, &CMyClientDlg::OnUpdateProgress)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyClientDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMyClientDlg ��Ϣ�������

BOOL CMyClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	EnabelServerElement(FALSE);
	m_ip = ntohl(inet_addr(GetHostIp()));
	m_port = SERVER_PORT_COMMAND;
	UpdateData(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMyClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyClientDlg::OnClose()
{
	if (m_pSocketData)
	{
		m_pSocketData->ShutDown();
		m_pSocketData->Close();
		delete m_pSocketData;
		m_pSocketData = NULL;
	}

	if (m_pSocketCommand)
	{
		m_pSocketCommand->ShutDown();
		m_pSocketCommand->Close();
		delete m_pSocketCommand;
		m_pSocketCommand = NULL;
	}

	CDialogEx::OnClose();
}

LRESULT CMyClientDlg::OnUpdateProgress(WPARAM wp ,LPARAM lp)
{
	CString str ;
	ULONGLONG len = wp;
	ULONGLONG mlen = lp;
	str.Format(L"push : %llu : %llu .",len,mlen);
	PrintMsg(str);
	return 0;
}

void CMyClientDlg::OnBnClickedOk()
{
	CString str = L"";
	char buf[256] = {0};

	int ret = GetWindowTextA(m_msgEdit.m_hWnd,buf,256);
	if(ret > 0)
	{
		str = buf;
		str.Trim();
		if (str.GetLength()>0)
		{
			MYMSG msg = {0};
			msg.id = My_Msg_Type_Msg;
			strcpy_s(msg.buf1,CStringA(str));
			if (m_pSocketCommand)
			{
				m_pSocketCommand->Send(&msg,sizeof(msg));
			}
		}
		m_msgEdit.Clear();
	}
}

void CMyClientDlg::PrintMsg(CString str)
{
	int nIndex = m_list.AddString(str);
	m_list.SetCurSel(nIndex);
}

void CMyClientDlg::OnProgress(CMySocket * pSocket)
{
	ULONGLONG len = ((CCltSocketData*)pSocket)->m_msg.len;
	ULONGLONG mlen = ((CCltSocketData*)pSocket)->m_nCommSize;

	PostMessage(WM_MY_PROGRESS,(WPARAM)len,(LPARAM)mlen);
}

void CMyClientDlg::OnSocketCommand(CMySocket * pSocket, MYMSG msg)
{
	switch(msg.id)
	{
	case My_Msg_Type_Msg:
		PrintMsg(CString(msg.buf1));
		break;
	case My_Msg_Type_Push_File:
		OnPushFile( pSocket, msg);
		break;
	case My_Msg_Type_Push_File_Ack:
		break;
	case My_Msg_Type_Push_File_Progress:
		break;
	case My_Msg_Type_Push_File_End:
		OnPushFileEnd( pSocket, msg);
		break;
	case My_Msg_Type_Push_File_End_Ack:
		break;
	default:
		ASSERT(0);	//not arrived
		break;
	}
}

void CMyClientDlg::EnabelServerElement(BOOL btrue)
{
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(!btrue);
	GetDlgItem(IDC_EDIT2)->EnableWindow(!btrue);

	GetDlgItem(IDC_BUTTON1)->EnableWindow(!btrue);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(btrue);
	GetDlgItem(IDOK)->EnableWindow(btrue);

	GetDlgItem(IDC_EDIT1)->EnableWindow(btrue);
}

char * CMyClientDlg::GetHostIp()
{
	char HostName[100];
	gethostname(HostName, sizeof(HostName));
	hostent* hn = gethostbyname(HostName);
	return inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);
}

int CMyClientDlg::ConnectServer()
{
	UpdateData();
	m_pSocketCommand = new CCltSocketCmd(SERVER_PORT_COMMAND);
	m_pSocketCommand->Create();

	sockaddr_in addr ;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(m_ip);
	addr.sin_port = htons(m_port);
	BOOL ret = m_pSocketCommand->Connect((SOCKADDR *)(&addr), sizeof(sockaddr_in));
	if(ret)
	{
		m_pSocketCommand->SetSocketInfo();
		return 0;
	}

	return 1;
}

int CMyClientDlg::StopConnect()
{
	if (m_pSocketData)
	{
		m_pSocketData->ShutDown();
		m_pSocketData->Close();
		delete m_pSocketData;
		m_pSocketData = NULL;
	}

	if (m_pSocketCommand)
	{
		m_pSocketCommand->ShutDown();
		m_pSocketCommand->Close();
		delete m_pSocketCommand;
		m_pSocketCommand = NULL;
	}
	return 0;
}

void CMyClientDlg::OnSrvClosed(int nErrorCode,UINT Port)
{
	if(Port == SERVER_PORT_COMMAND)
	{
		m_pSocketCommand->ShutDown();
		m_pSocketCommand->Close();
		delete m_pSocketCommand;
		m_pSocketCommand = NULL;
		PrintMsg(L"Server cmd Is Close......");
	}else if(Port == SERVER_PORT_DATA)
	{
		m_pSocketData->ShutDown();
		m_pSocketData->Close();
		delete m_pSocketData;
		m_pSocketData = NULL;
		PrintMsg(L"Server data Is Close......");
	}else
	{
		ASSERT(0);
	}
}

void CMyClientDlg::OnPushFile(CMySocket * pSocket, MYMSG msg)
{
	ASSERT(m_pSocketData==NULL);
	m_pSocketData = new CCltSocketData(SERVER_PORT_DATA);
	m_pSocketData->Create();

	sockaddr_in addr ;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(m_pSocketCommand->m_node.addr);
	addr.sin_port = htons(SERVER_PORT_DATA);
	BOOL ret = m_pSocketData->Connect((SOCKADDR *)(&addr), sizeof(sockaddr_in));
	if(ret)
	{
		MYMSG m = msg;
		m.id = My_Msg_Type_Push_File_Ack;
		PrintMsg(L"CCltSocketData ok......");
		m_pSocketData->SetSocketInfo();
		m.error = m_pSocketData->PreGetFile(msg);
		m_pSocketCommand->Send(&m,sizeof(m));
		PrintMsg(L"start get file......");
	}
}

void CMyClientDlg::OnPushFileEnd(CMySocket * pSocket, MYMSG msg)
{
	MYMSG m = msg;
	m.id = My_Msg_Type_Push_File_End_Ack;
	m.error = m_pSocketData->GetFileEnd();
	PrintMsg(L"end get file......");
	m_pSocketCommand->Send(&m,sizeof(m));
	m_pSocketData->Close();
	delete m_pSocketData;
	m_pSocketData = NULL;
	PrintMsg(L"close data socket......");
}


void CMyClientDlg::OnBnClickedButton1()
{
	int ret = ConnectServer();
	if(!ret)
	{
		EnabelServerElement(TRUE);
		PrintMsg(L"connectted to server......");
	}else
	{
		PrintMsg(L"connectted  eror......");
	}
}


void CMyClientDlg::OnBnClickedButton2()
{
	StopConnect();
	EnabelServerElement(FALSE);
}
