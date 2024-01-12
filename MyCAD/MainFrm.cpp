
// MainFrm.cpp : implementation of the MainFrame class
//

#include "pch.h"
#include "framework.h"
#include "MyCAD.h"
#include "MyCADView.h"
#include "MyCADDoc.h"
#include "MainFrm.h"
#include "Line.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// MainFrame

IMPLEMENT_DYNCREATE(MainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(MainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &MainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &MainFrame::OnToolbarCreateNew)
	ON_COMMAND(ID_DRAW_TYPE, &MainFrame::OnDrawType)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID__LINE, &MainFrame::OnLine)
	ON_COMMAND(ID__CIRCLE, &MainFrame::OnCircle)
	ON_UPDATE_COMMAND_UI(ID__LINE, &MainFrame::OnUpdateLine)
	ON_UPDATE_COMMAND_UI(ID__CIRCLE, &MainFrame::OnUpdateCircle)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// MainFrame construction/destruction

MainFrame::MainFrame() noexcept
{
	// TODO: add member initialization code here
}

MainFrame::~MainFrame()
{
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	// Load menu for dropdown
	if (!m_menuDrawType.LoadMenu(IDR_MENU_DRAW))
	{
		TRACE0("Failed to load line type menu\n");
		return -1; // fail to create
	}
	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndToolBarView.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarView.LoadToolBar(IDR_TOOLBAR_VIEW))
	{
		TRACE0("Failed to create toolbarview\n");
		return -1;      // fail to create
	}
	if (!m_wndToolBarZoom.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarZoom.LoadToolBar(IDR_TOOLBAR_ZOOM))
	{
		TRACE0("Failed to create toolbarview\n");
		return -1;      // fail to create
	}
	if (!m_wndToolBarDraw.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarDraw.LoadToolBar(IDR_TOOLBAR_DRAW))
	{
		TRACE0("Failed to create toolbar linetype\n");
		return -1;      // fail to create
	}
	

	// Add a dropdown button to the toolbar
	CMFCToolBarMenuButton menuButton(ID_DRAW_TYPE, m_menuDrawType.GetSubMenu(0)->GetSafeHmenu(),
		GetCmdMgr()->GetCmdImage(ID_DRAW_TYPE), _T("Line Type"));
	m_wndToolBarDraw.ReplaceButton(ID_DRAW_TYPE, menuButton);


	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarZoom.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarDraw.EnableDocking(CBRS_ALIGN_ANY);


	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);
	DockPane(&m_wndToolBarView);
	DockPane(&m_wndToolBarZoom);
	DockPane(&m_wndToolBarDraw);

	



	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == nullptr)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	/*CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);
	*/
	HBITMAP hbitmap = (HBITMAP)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SOLID), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	CMenu* pMenu = GetMenu();
	if (pMenu != NULL && hbitmap)
	{
		int nLast = pMenu->GetMenuItemCount();
		CMenu* sub = pMenu->GetSubMenu(nLast - 1);
		if (sub)
		{
			m_bitmap.Attach(hbitmap);
			sub->SetMenuItemBitmaps(ID_LINE_SOLID, MF_BYCOMMAND, &m_bitmap, &m_bitmap);
		}
	}

	return 0;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// MainFrame message handlers

void MainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT MainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}


BOOL MainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void MainFrame::OnDrawType()
{
	CRect rect;
	m_wndToolBarDraw.GetItemRect(m_wndToolBarDraw.CommandToIndex(ID_DRAW_TYPE), &rect);
	m_wndToolBarDraw.ClientToScreen(&rect);

	m_menuDrawType.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
		rect.left, rect.bottom, this, &rect);
}







void MainFrame::OnLine()
{
	// TODO: Add your command handler code here
	CMyCADView* pView = (CMyCADView*)GetActiveView();
	if (pView)
	{
		pView->SetCurrentMode(CMyCADView::LineType);
	}
}


void MainFrame::OnCircle()
{
	CMyCADView* pView = (CMyCADView*)GetActiveView();
	if (pView) {
		pView->SetCurrentMode(CMyCADView::CircleType);
	}
}


void MainFrame::OnUpdateLine(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void MainFrame::OnUpdateCircle(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

