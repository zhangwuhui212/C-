
// GetProcessCmdLineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GetProcessCmdLine.h"
#include "GetProcessCmdLineDlg.h"
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


// CGetProcessCmdLineDlg 对话框




CGetProcessCmdLineDlg::CGetProcessCmdLineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGetProcessCmdLineDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGetProcessCmdLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_process);
	DDX_Control(pDX, IDC_LIST2, m_list_process_info);
}

BEGIN_MESSAGE_MAP(CGetProcessCmdLineDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CGetProcessCmdLineDlg::OnItemchangedList1)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CGetProcessCmdLineDlg::OnNMRClickList1)
	ON_COMMAND(ID_REFRESH, &CGetProcessCmdLineDlg::OnRefresh)
END_MESSAGE_MAP()


// CGetProcessCmdLineDlg 消息处理程序

BOOL CGetProcessCmdLineDlg::OnInitDialog()
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
	
	LONG lStyle;
	lStyle = GetWindowLong(m_list_process.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_list_process.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = m_list_process.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	m_list_process.SetExtendedStyle(dwStyle); //设置扩展风格
	m_list_process.SetBkColor(RGB(166,210,210));
	m_list_process.SetTextColor(RGB(0,0,0));
	m_list_process.SetTextBkColor(RGB(166,210,210));

	LoadProcess();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#include "PSAPI.H"
#pragma   comment(lib,"psapi")  

BOOL GetProcessCommandLine(HANDLE hProcess, LPTSTR pszCmdLine, DWORD cchCmdLine)
{
	BOOL			bRet;
	DWORD			dwPos;
	LPBYTE			lpAddr;
	DWORD			dwRetLen;

	bRet = FALSE;

	dwPos = 0;
	lpAddr = (LPBYTE)GetCommandLine;
Win7:
	if(lpAddr[dwPos] == 0xeb && lpAddr[dwPos + 1] == 0x05)
	{
		dwPos += 2;
		dwPos += 5;
Win8:
		if(lpAddr[dwPos] == 0xff && lpAddr[dwPos + 1] == 0x25)
		{
			dwPos += 2;
			lpAddr = *(LPBYTE*)(lpAddr + dwPos);

			dwPos = 0;
			lpAddr = *(LPBYTE*)lpAddr;
WinXp:
			if(lpAddr[dwPos] == 0xa1)
			{
				dwPos += 1;
				lpAddr = *(LPBYTE*)(lpAddr + dwPos);
				bRet = ReadProcessMemory(hProcess,
					lpAddr,
					&lpAddr,
					sizeof(LPBYTE),
					&dwRetLen);
				if(bRet)
				{
					bRet = ReadProcessMemory(hProcess,
						lpAddr,
						pszCmdLine,
						cchCmdLine,
						&dwRetLen);
				}
			}
		}
		else
		{			
			goto WinXp;
		}
	}
	else
	{
		goto Win8;
	}

	return bRet;
}


void CGetProcessCmdLineDlg::LoadProcess()
{
	int nRow = -1;
	while ( m_list_process.DeleteColumn (0));
	m_list_process.InsertColumn( 0, L"名称", LVCFMT_LEFT, 150 );//插入列
	m_list_process.InsertColumn( 1, L"路径", LVCFMT_LEFT, 300 );
	m_list_process_info.ResetContent();

	DWORD aProcesses[1024], cbNeeded, cProcesses;    
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )  //receive all the Process ID Saved in aProcesses
	{
		::MessageBox(NULL,TEXT(""),TEXT("Can not EnumProsses"),0);
		return;
	}

	HANDLE hProcess = NULL;                //进程句柄
	TCHAR szProcessName[MAX_PATH] ={0};    //进程的名称
	TCHAR szProcessPath[MAX_PATH] ={0};    //进程路径
	HMODULE hMod=NULL;                    //进程的第一个模块
	DWORD ModulecbNeeded;                //储存进程模块的大小
	 
	cProcesses = cbNeeded / sizeof(DWORD);        //the total of Process ID
	for (int i = 0; i < cProcesses; i++ )
	{
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, aProcesses[i]);//get the handle of every Process which is Indentified by Process ID
		if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &ModulecbNeeded) )                // 列举某个进程的模块，第一个模块就是创建此进程的主exe程序 
		{
			if (    GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName) )    //获得程序名
				&& GetModuleFileNameEx( hProcess, hMod, szProcessPath, sizeof(szProcessPath))  )//获得模块路径
			{
				
				nRow = m_list_process.InsertItem(0,szProcessName);
				m_list_process.SetItemText(nRow,1,szProcessPath);
				m_list_process.SetItemData(nRow,(DWORD_PTR)aProcesses[i]);
			}
		}
		CloseHandle(hProcess);
	}  


}

void CGetProcessCmdLineDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGetProcessCmdLineDlg::OnPaint()
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
HCURSOR CGetProcessCmdLineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGetProcessCmdLineDlg::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString strProcess=L"";
	DWORD Pid  =0;
	HANDLE hProcess = NULL; 
	TCHAR szPath[4*MAX_PATH]={0}; 

	LPNMLISTVIEW pNMListView  = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CString sTemp;

	if((pNMListView->uOldState & LVIS_FOCUSED) == LVIS_FOCUSED &&
		(pNMListView->uNewState & LVIS_FOCUSED) == 0)
	{
		sTemp.Format(L"%d losted focus",pNMListView->iItem);
	}
	else if((pNMListView->uOldState & LVIS_FOCUSED) == 0 &&
		(pNMListView->uNewState & LVIS_FOCUSED) == LVIS_FOCUSED)
	{
		sTemp.Format(L"%d got focus",pNMListView->iItem);
	}

	if((pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED &&
		(pNMListView->uNewState & LVIS_SELECTED) == 0)
	{
		sTemp.Format(L"%d losted selected",pNMListView->iItem);
	}
	else if((pNMListView->uOldState & LVIS_SELECTED) == 0 &&
		(pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		m_list_process_info.ResetContent();
		sTemp.Format(L"%d got selected",pNMListView->iItem);
		strProcess =  m_list_process.GetItemText(pNMListView->iItem,0);
		Pid = m_list_process.GetItemData(pNMListView->iItem);
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, Pid);//get the handle of every Process which is Indentified by Process ID
		if(hProcess)
		{
			if(GetProcessCommandLine(hProcess, szPath, sizeof(szPath)))  
			{
				CString str;
				str.Format(L"名称： %s",strProcess);
				m_list_process_info.AddString(str);
				str.Format(L"PID： %d",Pid);
				m_list_process_info.AddString(str);
				str.Format(L"起始命令： %s",CString(szPath));
				m_list_process_info.AddString(str);
			} 
		}
		CloseHandle(hProcess);
	}

	*pResult = 0;
}




HBRUSH CGetProcessCmdLineDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	COLORREF clr;
	HBRUSH m_brMine;
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
		clr = RGB(138,138,138);
		m_brMine = ::CreateSolidBrush(clr);
		return m_brMine; 

	case CTLCOLOR_LISTBOX:
		clr = RGB(0,0,0);
		pDC->SetTextColor(clr);//设置黑色的文本

		clr = RGB(166,210,210);
		pDC->SetBkColor(clr);//设置淡蓝色的背景

		m_brMine = ::CreateSolidBrush(clr);
		return m_brMine; //返回对应的刷子句柄

	default:
		HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
	}
}


void CGetProcessCmdLineDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(pNMItemActivate->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

		CMenu menu;
		VERIFY( menu.LoadMenu( IDR_MENU1 ) );
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT( popup != NULL );
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	} 
	*pResult = 0;
}


void CGetProcessCmdLineDlg::OnRefresh()
{
	LoadProcess();
}
