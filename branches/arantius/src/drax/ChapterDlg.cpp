//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: ChapterDlg.cpp
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ChapterDlg.h"

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CChapterDlg::CChapterDlg(CWnd *ppwndParent, CMovieDoc *ppkDoc, int piChapter)
: CDialogEx(IDD_CHAPTER, ppwndParent),
  m_pkDoc(ppkDoc),
  m_iChapter(piChapter)
{
	ASSERT_VALID_PTR(ppwndParent);
	ASSERT_VALID_PTR(ppkDoc);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CChapterDlg::~CChapterDlg()
{
}

BEGIN_MESSAGE_MAP(CChapterDlg, CDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CChapterDlg::OnInitDialog()
{
	if ( !CDialogEx::OnInitDialog() )
		return FALSE;

	// enable mask
	m_wndTime.EnableMask(_T("DD DD DD DDD"), _T("__:__:__.___"), _T('0'), _T("1234567890"));
	m_wndTime.EnableGetMaskedCharsOnly(FALSE);

	// without current chapter
	if ( -1 == m_iChapter )
	{
		// init
		m_wndTime.SetWindowText(_T("00:00:00.000"));
		m_wndName.SetWindowText(_T(""));
	}
	// with current chapter
	else
	{
		CChapter *lpkChapter = m_pkDoc->m_kChapters[m_iChapter];
		ASSERT_VALID_PTR(lpkChapter);

		// init
		m_wndTime.SetWindowText(lpkChapter->m_kTime.ToString());
		m_wndName.SetWindowText(lpkChapter->m_sName);
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChapterDlg::DoDataExchange(CDataExchange *ppkDX)
{
	DDX_Control(ppkDX, IDC_EDIT_TIME, m_wndTime);
	DDX_Control(ppkDX, IDC_EDIT_NAME, m_wndName);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CChapterDlg::OnOK()
{
	if ( !UpdateData() )
		return;

	// get time
	CString lsTime;
	m_wndTime.GetWindowText(lsTime);

	// get name
	CString lsName;
	m_wndName.GetWindowText(lsName);

	// add or edit
	if ( !m_pkDoc->AddEditChapter(m_iChapter, lsTime, lsName) )
		return;

	// update view
	m_pkDoc->UpdateAllViews(NULL);
	m_pkDoc->SetModifiedFlag();
	CDialogEx::OnOK();
}
