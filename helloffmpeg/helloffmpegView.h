
// helloffmpegView.h : ChelloffmpegView ��Ľӿ�
//

#pragma once


class ChelloffmpegView : public CView
{
protected: // �������л�����
	ChelloffmpegView();
	DECLARE_DYNCREATE(ChelloffmpegView)

// ����
public:
	ChelloffmpegDoc* GetDocument() const;

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
	virtual ~ChelloffmpegView();
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

#ifndef _DEBUG  // helloffmpegView.cpp �еĵ��԰汾
inline ChelloffmpegDoc* ChelloffmpegView::GetDocument() const
   { return reinterpret_cast<ChelloffmpegDoc*>(m_pDocument); }
#endif

