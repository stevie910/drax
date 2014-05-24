//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: ChaptersView.cpp
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ChaptersView.h"

#include "ChapterDlg.h"

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CChaptersView::CChaptersView()
{
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CChaptersView::~CChaptersView()
{
}

IMPLEMENT_DYNCREATE(CChaptersView, CListView)

BEGIN_MESSAGE_MAP(CChaptersView, CListView)
	ON_COMMAND(ID_CHAPTER_ADD, OnChapterAdd)
	ON_COMMAND(ID_CHAPTER_EDIT, OnChapterEdit)
	ON_UPDATE_COMMAND_UI(ID_CHAPTER_EDIT, OnUpdateChapterEdit)
	ON_COMMAND(ID_CHAPTER_REMOVE, OnChapterRemove)
	ON_UPDATE_COMMAND_UI(ID_CHAPTER_REMOVE, OnUpdateChapterRemove)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClk)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnInitialUpdate()
{
	// get list control
	CListCtrl &lwndList = GetListCtrl();

	// change style
	lwndList.ModifyStyle(NULL, LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_NOSORTHEADER);
	lwndList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

	CRect lkRect;
	GetClientRect(lkRect);
			
	// add columns
	lwndList.InsertColumn(1, AfxStringID(IDS_ID), LVCFMT_CENTER, (int)(lkRect.Width() * 0.1), 0);
	lwndList.InsertColumn(2, AfxStringID(IDS_TIME), LVCFMT_CENTER, (int)(lkRect.Width() * 0.2), 1);
	lwndList.InsertColumn(3, AfxStringID(IDS_NAME), LVCFMT_LEFT, (int)(lkRect.Width() * 0.7) - 25, 2);

	CListView::OnInitialUpdate();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnChapterAdd()
{
	// add chapter
	CChapterDlg ldlgChapter(AfxGetMainWnd(), GetDocument());
	ldlgChapter.DoModal();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnChapterEdit()
{
	// edit chapter
	CChapterDlg ldlgChapter(AfxGetMainWnd(), GetDocument(), GetListCtrl().GetSelectionMark());
	ldlgChapter.DoModal();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnUpdateChapterEdit(CCmdUI *ppkCmdUI)
{
	ppkCmdUI->Enable(GetListCtrl().GetSelectionMark() >= 0);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnChapterRemove()
{
	CMovieDoc *lpkDoc = GetDocument();
	ASSERT_VALID_PTR(lpkDoc);

	// remove chapter
	if ( !lpkDoc->RemoveChapter(GetListCtrl().GetSelectionMark()) )
		return;

	// update view
	lpkDoc->UpdateAllViews(NULL);
	lpkDoc->SetModifiedFlag();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnUpdateChapterRemove(CCmdUI *ppkCmdUI)
{
	ppkCmdUI->Enable(GetListCtrl().GetSelectionMark() >= 0);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnUpdate(CView *ppwndSender, LPARAM plHint, CObject *ppkHint)
{
	// get list control
	CListCtrl &lwndList = GetListCtrl();

	// clear list
	VERIFY(lwndList.DeleteAllItems());

	// fill list
	for ( int i = 0; i < GetDocument()->m_kChapters.GetSize(); i++ )
	{
		// get chapter
		CChapter *lpkChapter = GetDocument()->m_kChapters[i];

		// add element
		int liItem = lwndList.InsertItem(i, AfxIntToString(i + 1));
		lwndList.SetItemText(liItem, 1, lpkChapter->m_kTime.ToString());
		lwndList.SetItemText(liItem, 2, lpkChapter->m_sName);
		lwndList.SetItemData(liItem, (DWORD_PTR)lpkChapter);
	}
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChaptersView::OnDblClk(NMHDR *pptNotify, LRESULT *plResult)
{
	if ( GetListCtrl().GetSelectionMark() >= 0 )
		OnChapterEdit();

	*plResult = 0;
}
