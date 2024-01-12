
// MyCADView.cpp : implementation of the CMyCADView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MyCAD.h"
#endif

#include "MyCADDoc.h"
#include "MyCADView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyCADView

IMPLEMENT_DYNCREATE(CMyCADView, CView)

BEGIN_MESSAGE_MAP(CMyCADView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMyCADView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_BUTTON_VIEWTOP, &CMyCADView::OnButtonViewtop)
	ON_COMMAND(ID_BUTTON_VIEWBOTTOM, &CMyCADView::OnButtonViewbottom)
	ON_COMMAND(ID_BUTTON_VIEWLEFT, &CMyCADView::OnButtonViewleft)
	ON_COMMAND(ID_BUTTON_VIEWRIGHT, &CMyCADView::OnButtonViewright)
	ON_COMMAND(ID_BUTTON_VIEWFRONT, &CMyCADView::OnButtonViewfront)
	ON_COMMAND(ID_BUTTON_VIEWBACK, &CMyCADView::OnButtonViewback)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEWTOP, &CMyCADView::OnUpdateButtonViewtop)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEWBOTTOM, &CMyCADView::OnUpdateButtonViewbottom)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEWLEFT, &CMyCADView::OnUpdateButtonViewleft)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEWRIGHT, &CMyCADView::OnUpdateButtonViewright)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEWFRONT, &CMyCADView::OnUpdateButtonViewfront)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEWBACK, &CMyCADView::OnUpdateButtonViewback)
	ON_COMMAND(ID_BUTTON_ZOOMOUT, &CMyCADView::OnZoomOut)
	ON_COMMAND(ID_BUTTON_ZOOMIN, &CMyCADView::OnZoomIn)
	ON_COMMAND(ID_BUTTON_RESET, &CMyCADView::OnZoomReset)
	ON_COMMAND(ID_BUTTON_FULLLSCREEN, &CMyCADView::OnButtonFulllscreen)
	
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CMyCADView construction/destruction

CMyCADView::CMyCADView() noexcept
	:m_zoomFactor(1.0),m_zoomCenter(0,0)

{
	// TODO: add construction code here
	m_bDrawing = false;

}

CMyCADView::~CMyCADView()
{
	// Giải phóng back buffer
	if (m_backBuffer.GetSafeHandle())
		m_backBuffer.DeleteObject();
}

BOOL CMyCADView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMyCADView drawing
void CMyCADView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_currentMode == LineType || m_currentMode == CircleType) {
		m_bDrawing = true;
		m_ptStart = point;
		m_ptCurrent = point;
		SetCapture(); // theo doi chuot
	}


	CView::OnLButtonDown(nFlags, point);
}

void CMyCADView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDrawing && (m_currentMode == LineType||m_currentMode==CircleType)) {
		m_ptCurrent = point;
		Invalidate(); // ve duong thang tam thoi
	}

	CView::OnMouseMove(nFlags, point);
}



void CMyCADView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDrawing) {
		if (m_currentMode == LineType) {
			// them line vao danh sach
			m_lines.push_back(Line(m_ptStart, point));
		}
		else if (m_currentMode==CircleType)
		{
			int radius = static_cast<int>(sqrt(pow(m_ptCurrent.x - m_ptStart.x, 2) + pow(m_ptCurrent.y - m_ptStart.y, 2)));           
			// Thêm hình tròn vào danh sách (Bạn cần cấu trúc lưu trữ cho hình tròn)
			 m_circles.push_back(Circle(m_ptStart, radius));
		}
		m_bDrawing = false;
		ReleaseCapture(); // bo theo doi chuo
		Invalidate();// ve lai cua so

	}
	CView::OnLButtonUp(nFlags, point);
}

void CMyCADView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// tao back buffer
	if (cx > 0 && cy > 0) {
		CClientDC dc(this);
		if (m_backBuffer.GetSafeHandle() != NULL) {
			m_backBuffer.DeleteObject();
		}
		m_backBuffer.CreateCompatibleBitmap(&dc, cx, cy);
	}

}


void CMyCADView::OnDraw(CDC* pDC)
{
	CMyCADDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// tao  memory de ve len bitmap
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = memDC.SelectObject(&m_backBuffer);
	// ve nen trang
	CRect rect;
	GetClientRect(&rect);
	memDC.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), WHITENESS); // xoa background cuar back buffer

	// ve cac doi tuong len back buffer
	// ve cac line len memory thay vi screen
	for (const auto& Line : m_lines) {
		memDC.MoveTo(Line.start);
		memDC.LineTo(Line.end);
	}

	// ve cac circle len memory thay vi screen

	for (const auto& circle : m_circles) {
		memDC.Ellipse(circle.center.x - circle.radius, circle.center.y - circle.radius,
			circle.center.x + circle.radius, circle.center.y + circle.radius);
	}

	// ve hinh hien tai len memory dc
	if (m_bDrawing) {
		
		if (m_currentMode == LineType) {
			memDC.MoveTo(m_ptStart);
			memDC.LineTo(m_ptCurrent);
		}
		else if (m_currentMode == CircleType) {
			int radius = static_cast<int>(sqrt(pow(m_ptCurrent.x - m_ptStart.x, 2) +
				pow(m_ptCurrent.y - m_ptStart.y, 2)));
			memDC.Ellipse(m_ptStart.x - radius, m_ptStart.y - radius,
				m_ptStart.x + radius, m_ptStart.y + radius);
		}
		
	}
	// chep context tu memory sang screen
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, rect.left, rect.top, SRCCOPY);

	// Dọn dẹp: chọn lại bitmap cũ vào memory DC
	memDC.SelectObject(pOldBitmap);
}



// CMyCADView printing


void CMyCADView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMyCADView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMyCADView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMyCADView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMyCADView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMyCADView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMyCADView diagnostics

#ifdef _DEBUG
void CMyCADView::AssertValid() const
{
	CView::AssertValid();
}

void CMyCADView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyCADDoc* CMyCADView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyCADDoc)));
	return (CMyCADDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyCADView message handlers


void CMyCADView::OnButtonViewtop()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnButtonViewbottom()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnButtonViewleft()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnButtonViewright()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnButtonViewfront()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnButtonViewback()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnUpdateButtonViewtop(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CMyCADView::OnUpdateButtonViewbottom(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CMyCADView::OnUpdateButtonViewleft(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CMyCADView::OnUpdateButtonViewright(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CMyCADView::OnUpdateButtonViewfront(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CMyCADView::OnUpdateButtonViewback(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CMyCADView::OnButtonZoom()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnZoomOut()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnZoomIn()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnZoomReset()
{
	// TODO: Add your command handler code here
}


void CMyCADView::OnButtonFulllscreen()
{
	// TODO: Add your command handler code here
}




BOOL CMyCADView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default



	if (nFlags & MK_CONTROL)
	{
		if (zDelta > 0) {
			m_zoomFactor *= 1.1;
		}
		else if(zDelta<0)
		{ 
			m_zoomFactor /= 1.1;

		}
		ScreenToClient(&pt);// Chuyển vị trí con trỏ từ tọa độ màn hình sang tọa độ cửa sổ
		m_zoomCenter = pt;  // Đặt làm trung tâm mới cho việc phóng to/thu nhỏ
		Invalidate(); //ve lai 
		return TRUE;
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);


}
