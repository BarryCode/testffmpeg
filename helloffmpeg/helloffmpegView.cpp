
// helloffmpegView.cpp : ChelloffmpegView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "helloffmpeg.h"
#endif

#include "helloffmpegDoc.h"
#include "helloffmpegView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChelloffmpegView

IMPLEMENT_DYNCREATE(ChelloffmpegView, CView)

BEGIN_MESSAGE_MAP(ChelloffmpegView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &ChelloffmpegView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// ChelloffmpegView ����/����

ChelloffmpegView::ChelloffmpegView()
{
	// TODO: �ڴ˴���ӹ������

}

ChelloffmpegView::~ChelloffmpegView()
{
}

BOOL ChelloffmpegView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// ChelloffmpegView ����

void ChelloffmpegView::OnDraw(CDC* /*pDC*/)
{
	ChelloffmpegDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// ChelloffmpegView ��ӡ


void ChelloffmpegView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL ChelloffmpegView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void ChelloffmpegView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void ChelloffmpegView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void ChelloffmpegView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void ChelloffmpegView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// ChelloffmpegView ���

#ifdef _DEBUG
void ChelloffmpegView::AssertValid() const
{
	CView::AssertValid();
}

void ChelloffmpegView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

ChelloffmpegDoc* ChelloffmpegView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ChelloffmpegDoc)));
	return (ChelloffmpegDoc*)m_pDocument;
}
#endif //_DEBUG


// ChelloffmpegView ��Ϣ�������
