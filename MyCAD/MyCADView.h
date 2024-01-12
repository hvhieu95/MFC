
// MyCADView.h : interface of the CMyCADView class
//

#pragma once
#include "MyCADDoc.h"
#include "Line.h"
#include "Circle.h"
#include <vector>



class CMyCADView : public CView
{
protected: // create from serialization only
	CMyCADView() noexcept;
	DECLARE_DYNCREATE(CMyCADView)
protected: 
	CBitmap   m_backBuffer;  
// Attributes
public:
	CMyCADDoc* GetDocument() const;
public:
	enum  DrawingMode { None, LineType, CircleType };
private:
	std::vector<Line> m_lines; // danh sach luu tru cac line
	std::vector<Circle> m_circles; // luu danh sach cac circle
private:
	
	DrawingMode   m_currentMode;
	bool          m_bDrawing;
	CPoint        m_ptStart;
	CPoint        m_ptCurrent;
	double        m_zoomFactor;
	CPoint        m_zoomCenter;
// Operations
public:
	void SetCurrentMode(DrawingMode mode) { m_currentMode = mode; }
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMyCADView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnButtonViewtop();
	afx_msg void OnButtonViewbottom();
	afx_msg void OnButtonViewleft();
	afx_msg void OnButtonViewright();
	afx_msg void OnButtonViewfront();
	afx_msg void OnButtonViewback();
	afx_msg void OnUpdateButtonViewtop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonViewbottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonViewleft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonViewright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonViewfront(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonViewback(CCmdUI* pCmdUI);
	afx_msg void OnButtonZoom();
	afx_msg void OnZoomOut();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomReset();
	afx_msg void OnButtonFulllscreen();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // debug version in MyCADView.cpp
inline CMyCADDoc* CMyCADView::GetDocument() const
   { return reinterpret_cast<CMyCADDoc*>(m_pDocument); }
#endif

