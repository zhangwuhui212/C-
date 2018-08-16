
// InternationalLanguageView.cpp : CInternationalLanguageView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "InternationalLanguage.h"
#endif

#include "InternationalLanguageDoc.h"
#include "InternationalLanguageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInternationalLanguageView

IMPLEMENT_DYNCREATE(CInternationalLanguageView, CView)

BEGIN_MESSAGE_MAP(CInternationalLanguageView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CInternationalLanguageView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CInternationalLanguageView 构造/析构

CInternationalLanguageView::CInternationalLanguageView()
{
	// TODO: 在此处添加构造代码

}

CInternationalLanguageView::~CInternationalLanguageView()
{
}

BOOL CInternationalLanguageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CInternationalLanguageView 绘制

void CInternationalLanguageView::OnDraw(CDC* /*pDC*/)
{
	CInternationalLanguageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CInternationalLanguageView 打印


void CInternationalLanguageView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CInternationalLanguageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CInternationalLanguageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CInternationalLanguageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CInternationalLanguageView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CInternationalLanguageView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CInternationalLanguageView 诊断

#ifdef _DEBUG
void CInternationalLanguageView::AssertValid() const
{
	CView::AssertValid();
}

void CInternationalLanguageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CInternationalLanguageDoc* CInternationalLanguageView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CInternationalLanguageDoc)));
	return (CInternationalLanguageDoc*)m_pDocument;
}
#endif //_DEBUG


// CInternationalLanguageView 消息处理程序
