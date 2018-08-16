
// InternationalLanguageView.h : CInternationalLanguageView ��Ľӿ�
//

#pragma once


class CInternationalLanguageView : public CView
{
protected: // �������л�����
	CInternationalLanguageView();
	DECLARE_DYNCREATE(CInternationalLanguageView)

// ����
public:
	CInternationalLanguageDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CInternationalLanguageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // InternationalLanguageView.cpp �еĵ��԰汾
inline CInternationalLanguageDoc* CInternationalLanguageView::GetDocument() const
   { return reinterpret_cast<CInternationalLanguageDoc*>(m_pDocument); }
#endif

