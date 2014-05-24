//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: MainFrame.cpp
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MainFrame.h"
#include "DropTarget.h"

// status bar indicators
static UINT miIndicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL
};

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
}

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateToClip)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateToClip)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateFromClip)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::IsEdit(CWnd *ppWnd)
{
	ASSERT_VALID_PTR(ppWnd);
	HWND lhWnd = ppWnd->GetSafeHwnd();

	if ( !VALID_PTR(lhWnd) )
		return FALSE;

	TCHAR lszClassName[6];
	return ::GetClassName(lhWnd, lszClassName, 6) && _tcsicmp(lszClassName, _T("Edit")) == 0;
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateToClip(CCmdUI *ppkCmdUI)
{
	CWnd *lpWnd = GetFocus();
	if ( !VALID_PTR(lpWnd) || !IsEdit(lpWnd) )
	{
		ppkCmdUI->Enable(FALSE);
	}
	else
	{
		CEdit *lpwndEdit = static_cast<CEdit*>(lpWnd);
		int liBeg, liEnd;

		lpwndEdit->GetSel(liBeg, liEnd);
		ppkCmdUI->Enable(liBeg != liEnd);
   }
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFromClip(CCmdUI *ppkCmdUI)
{
	CWnd *lpWnd = GetFocus();
	if ( !VALID_PTR(lpWnd) || !IsEdit(lpWnd) || (lpWnd->GetStyle() & ES_READONLY) != 0 )
		ppkCmdUI->Enable(FALSE);
	else
		ppkCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT pptCreate)
{
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
	if ( -1 == CMDIFrameWndEx::OnCreate(pptCreate) )
		return -1;

	// create menu bar
    if ( !m_wndMenuBar.Create(this) )
		return -1;

	// create tool bar
	if ( !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME) )
		return -1;

	// create status bar
	if ( !m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(miIndicators, sizeof(miIndicators)/sizeof(UINT)) )
		return -1;

	// enable docking
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	// Enable drag/drop.
	CDropTarget *pDropTarget = new CDropTarget(this->m_hWnd);
	// acquire a strong lock
	CoLockObjectExternal(pDropTarget, TRUE, FALSE);
	// tell OLE that the window is a drop target
	RegisterDragDrop(this->m_hWnd, pDropTarget);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditCut()
{
	CEdit *lpwndEdit = static_cast<CEdit*>(GetFocus());
	ASSERT(IsEdit(lpwndEdit));
	lpwndEdit->Cut();
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditCopy()
{
	CEdit *lpwndEdit = static_cast<CEdit*>(GetFocus());
	ASSERT(IsEdit(lpwndEdit));
	lpwndEdit->Copy();
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditPaste()
{
	CEdit *lpwndEdit = static_cast<CEdit*>(GetFocus());
	ASSERT(IsEdit(lpwndEdit));
	ASSERT(::IsClipboardFormatAvailable(CF_TEXT));
	lpwndEdit->Paste();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnClose()
{
	WINDOWPLACEMENT wp = { 0 };
	wp.length = sizeof(wp);
	if (GetWindowPlacement(&wp))
	{
		//if (!IsZoomed())
		//{
		//	// Work around; Aero Snap is invisible to GetWindowPlacement.
		//	GetWindowRect(&wp.rcNormalPosition);
		//}
		AfxGetApp()->WriteProfileBinary(L"MainFrame", L"WP", (LPBYTE)&wp, sizeof(wp));
	}

	CMDIFrameWndEx::OnClose();
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMDIFrameWnd::OnShowWindow(bShow, nStatus);

	static bool bOnce = true;

	if (bShow && !IsWindowVisible()
		&& bOnce)
	{
		bOnce = false;

		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(L"MainFrame", L"WP", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
}
