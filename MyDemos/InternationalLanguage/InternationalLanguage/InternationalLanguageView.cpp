
// InternationalLanguageView.cpp : CInternationalLanguageView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CInternationalLanguageView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CInternationalLanguageView ����/����

CInternationalLanguageView::CInternationalLanguageView()
{
	// TODO: �ڴ˴���ӹ������

}

CInternationalLanguageView::~CInternationalLanguageView()
{
}

BOOL CInternationalLanguageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CInternationalLanguageView ����

void CInternationalLanguageView::OnDraw(CDC* /*pDC*/)
{
	CInternationalLanguageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CInternationalLanguageView ��ӡ


void CInternationalLanguageView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CInternationalLanguageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CInternationalLanguageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CInternationalLanguageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
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


// CInternationalLanguageView ���

#ifdef _DEBUG
void CInternationalLanguageView::AssertValid() const
{
	CView::AssertValid();
}

void CInternationalLanguageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CInternationalLanguageDoc* CInternationalLanguageView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CInternationalLanguageDoc)));
	return (CInternationalLanguageDoc*)m_pDocument;
}
#endif //_DEBUG


// CInternationalLanguageView ��Ϣ�������
