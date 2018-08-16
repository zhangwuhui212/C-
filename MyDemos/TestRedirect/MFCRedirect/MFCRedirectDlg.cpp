
// MFCRedirectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCRedirect.h"
#include "MFCRedirectDlg.h"
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


// CMFCRedirectDlg 对话框




CMFCRedirectDlg::CMFCRedirectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCRedirectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCRedirectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CMFCRedirectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCRedirectDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFCRedirectDlg 消息处理程序

BOOL CMFCRedirectDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCRedirectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCRedirectDlg::OnPaint()
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
HCURSOR CMFCRedirectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMFCRedirectDlg::RunCMD()//最后这个参数说明是否重定向到文件
{
	HANDLE   hSTDINWrite, hSTDINRead;       // 用于重定向子进程输入的句柄   
	HANDLE   hSTDOUTWrite, hSTDOUTRead;     // 用于重定向子进程输出的句柄   
	SECURITY_ATTRIBUTES   sa;   

	sa.bInheritHandle = TRUE;   
	sa.lpSecurityDescriptor = NULL;   
	sa.nLength = sizeof(sa);   

	// 创建子进程输出匿名管道   
	if( !CreatePipe(&hSTDOUTRead, &hSTDOUTWrite, &sa, 0) )   
	{   
		m_list.AddString(L"Create   STDOUT   pipe   failed");  
		return FALSE;   
	}  

	// 创建子进程输入匿名管道   
	if( !CreatePipe(&hSTDINRead, &hSTDINWrite, &sa, 0))   
	{   
		m_list.AddString(L"Create   STDIN   pipe   failed"); 
		return FALSE;   
	}
	
	PROCESS_INFORMATION  pi;   
	ZeroMemory(&pi, sizeof(pi));   
	STARTUPINFO  si; 
	GetStartupInfo(&si);

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES   |   STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput   =   hSTDINRead;      //重定向子进程输入   
	si.hStdOutput   =   hSTDOUTWrite;   // 重定向子进程输入    
	si.hStdError = hSTDOUTWrite; //GetStdHandle( STD_ERROR_HANDLE ); 

	wchar_t cmd[20] = {0};
	wcscpy_s(cmd,L"ToHello.exe rd hl");
	if( !::CreateProcess(NULL, cmd, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi) )   
	{   
		int errorNo = ::GetLastError();
		m_list.AddString(L"create process failed");   
		return FALSE;   
	}

	::CloseHandle(hSTDOUTWrite);   
	::CloseHandle(hSTDINRead);

	char strData[1024] = {0, };
	DWORD dwBytes;
	while(::ReadFile(hSTDOUTRead, strData, sizeof(strData), &dwBytes, NULL))
	{   
		strData[dwBytes] = '\0';
		m_list.AddString(CString(strData));  
	}   

	::WaitForSingleObject(pi.hProcess, INFINITE);   
	::CloseHandle(hSTDOUTRead);   
	::CloseHandle(hSTDINWrite);   
	::CloseHandle(pi.hProcess);   
	::CloseHandle(pi.hThread); 

	return TRUE;
}

void CMFCRedirectDlg::OnBnClickedOk()
{
	TCHAR strLog[1024]={0};

	LPWSTR strCmp =_T("hello.exe abc>abc.h");
	RunCMD();
}
