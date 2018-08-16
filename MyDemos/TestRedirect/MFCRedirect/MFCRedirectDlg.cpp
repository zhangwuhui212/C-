
// MFCRedirectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCRedirect.h"
#include "MFCRedirectDlg.h"
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


// CMFCRedirectDlg �Ի���




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


// CMFCRedirectDlg ��Ϣ�������

BOOL CMFCRedirectDlg::OnInitDialog()
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCRedirectDlg::OnPaint()
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
HCURSOR CMFCRedirectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMFCRedirectDlg::RunCMD()//����������˵���Ƿ��ض����ļ�
{
	HANDLE   hSTDINWrite, hSTDINRead;       // �����ض����ӽ�������ľ��   
	HANDLE   hSTDOUTWrite, hSTDOUTRead;     // �����ض����ӽ�������ľ��   
	SECURITY_ATTRIBUTES   sa;   

	sa.bInheritHandle = TRUE;   
	sa.lpSecurityDescriptor = NULL;   
	sa.nLength = sizeof(sa);   

	// �����ӽ�����������ܵ�   
	if( !CreatePipe(&hSTDOUTRead, &hSTDOUTWrite, &sa, 0) )   
	{   
		m_list.AddString(L"Create   STDOUT   pipe   failed");  
		return FALSE;   
	}  

	// �����ӽ������������ܵ�   
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
	si.hStdInput   =   hSTDINRead;      //�ض����ӽ�������   
	si.hStdOutput   =   hSTDOUTWrite;   // �ض����ӽ�������    
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
