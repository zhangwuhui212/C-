
// ToExcelDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ToExcel.h"
#include "ToExcelDlg.h"
#include "afxdialogex.h"

#include "CApplication.h"
#include "CFont0.h"
#include "Cnterior.h"
#include "CRange.h"
#include "CRanges.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
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


// CToExcelDlg �Ի���




CToExcelDlg::CToExcelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CToExcelDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CToExcelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CToExcelDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CToExcelDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CToExcelDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CToExcelDlg ��Ϣ�������

BOOL CToExcelDlg::OnInitDialog()
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

	
	LONG lStyle;
	lStyle = GetWindowLong(m_list.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
	lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
	lStyle |= LVS_REPORT; //����style
	SetWindowLong(m_list.m_hWnd, GWL_STYLE, lStyle);//����style

	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
	m_list.SetExtendedStyle(dwStyle); //������չ���

	m_list.InsertColumn( 0, L"ID", LVCFMT_LEFT, 40 );//������
	m_list.InsertColumn( 1, L"NAME", LVCFMT_LEFT, 50 );
	int nRow = -1;
	for (int i=0;i<10;i++)
	{
		CString str;
		str.Format(L"ID[%d]",i+1);
		nRow = m_list.InsertItem(i, str);//������
		str.Format(L"Jack[%d]",i+1);
		m_list.SetItemText(nRow, 1, str);//��������
	}


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CToExcelDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CToExcelDlg::OnPaint()
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
HCURSOR CToExcelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void   GetCellName(int nRow, int nCol, CString &strName)
{
	int nSeed = nCol;
	CString strRow;
	char cCell = 'A' + nCol - 1;
	strName.Format(_T("%c"), cCell);
	strRow.Format(_T( "%d "), nRow);
	strName += strRow;
}

void CToExcelDlg::OnBnClickedButton1()
{
	CString strFile = _T("D:\\Test.xls");
	COleVariant covTrue((short)TRUE),
				covFalse((short)FALSE),
				covOptional((long)DISP_E_PARAMNOTFOUND,   VT_ERROR);

	CApplication   app;
	CWorkbooks   books;
	CWorkbook   book;
	CWorksheets   sheets;
	CWorksheet   sheet;
	CRange   range;
	CFont0   font;

	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		MessageBox(_T("����ʧ�ܣ�"));
		return;
	}

	books = app.get_Workbooks();
	book = books.Add(covOptional);
	sheets = book.get_Worksheets();
	sheet = sheets.get_Item(COleVariant((short)1));

	CHeaderCtrl   *pmyHeaderCtrl;
	pmyHeaderCtrl = m_list.GetHeaderCtrl();//�˾�ȡ��CListCtrl�ؼ����б��^

	int   iRow,iCol;
	int   m_cols   =   pmyHeaderCtrl-> GetItemCount();
	int   m_rows = m_list.GetItemCount();
	HDITEM   hdi;
	TCHAR     lpBuffer[256];
	bool       fFound   =   false;
	hdi.mask   =   HDI_TEXT;
	hdi.pszText   =   lpBuffer;
	hdi.cchTextMax   =   256;
	CString   colname;
	CString strTemp;
	for(iCol=0;   iCol <m_cols;   iCol++)//���б�ı���ͷд��EXCEL
	{
		GetCellName(1 ,iCol + 1, colname);
		range   =   sheet.get_Range(COleVariant(colname),COleVariant(colname));
		pmyHeaderCtrl-> GetItem(iCol,   &hdi);
		range.put_Value2(COleVariant(hdi.pszText));
		
		int   nWidth   =   m_list.GetColumnWidth(iCol)/6;
		//�õ���iCol+1��
		range.AttachDispatch(range.get_Item(_variant_t((long)(iCol+1)),vtMissing).pdispVal,true);
		//�����п� 
		range.put_ColumnWidth(_variant_t((long)nWidth));
	}
	Cnterior it;
	range   =   sheet.get_Range(COleVariant( _T("A1 ")),   COleVariant(colname));
	range.put_RowHeight(_variant_t((long)50));//�����еĸ߶�
	font = range.get_Font();
	font.put_Bold(covTrue);
	font.put_ColorIndex(_variant_t((long)40));
	range.put_VerticalAlignment(COleVariant((short)-4108));//xlVAlignCenter   =   -4108
	it.AttachDispatch(range.get_Interior()); 
	it.put_ColorIndex(_variant_t((long)24));  

	COleSafeArray   saRet;
	DWORD   numElements[]={m_rows,m_cols};//5x2   element   array
	saRet.Create(VT_BSTR,   2,   numElements);
	range   =   sheet.get_Range(COleVariant( _T("A2 ")),covOptional);
	range = range.get_Resize(COleVariant((short)m_rows),COleVariant((short)m_cols));
	long   index[2];
	range   =   sheet.get_Range(COleVariant( _T("A2 ")),covOptional);
	range   =   range.get_Resize(COleVariant((short)m_rows),COleVariant((short)m_cols));
	for(   iRow   =   1;   iRow   <=   m_rows;   iRow++)//���б�����д��EXCEL
	{
		for(   iCol   =   1;   iCol   <=   m_cols;   iCol++)
		{
			index[0]=iRow-1;
			index[1]=iCol-1;
			CString   szTemp;
			szTemp= m_list.GetItemText(iRow-1,iCol-1);
			BSTR   bstr   =   szTemp.AllocSysString();
			saRet.PutElement(index,bstr);
			SysFreeString(bstr);
		}
	}
	range.put_Value2(COleVariant(saRet));

	saRet.Detach();
	book.SaveCopyAs(COleVariant(strFile));
	book.put_Saved(true);

	book.ReleaseDispatch(); 
	books.ReleaseDispatch();
	app.Quit();
	app.ReleaseDispatch();
}


void CToExcelDlg::OnBnClickedButton2()
{
	
}
