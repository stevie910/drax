//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: InformationView.cpp
// Date......: 11/24/2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "InformationView.h"

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CInformationView::CInformationView()
{
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CInformationView::~CInformationView()
{
}

IMPLEMENT_DYNCREATE(CInformationView, CView)

BEGIN_MESSAGE_MAP(CInformationView, CView)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_SIZE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CInformationView::OnInitialUpdate()
{
	// create property grid
	VERIFY(m_wndProperties.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 0));

	// get document
	CMovieDoc *lpkDoc = GetDocument();
	ASSERT_VALID_PTR(lpkDoc);

	// add properties
	m_wndProperties.AddProperty(lpkDoc->m_pkAlbum);
	m_wndProperties.AddProperty(lpkDoc->m_pkAlbumArtist);
	m_wndProperties.AddProperty(lpkDoc->m_pkArtist);
	m_wndProperties.AddProperty(lpkDoc->m_pkComments);
	m_wndProperties.AddProperty(lpkDoc->m_pkCompilation);
	m_wndProperties.AddProperty(lpkDoc->m_pkComposer);
	m_wndProperties.AddProperty(lpkDoc->m_pkDisk);
	m_wndProperties.AddProperty(lpkDoc->m_pkTotalDisks);
	m_wndProperties.AddProperty(lpkDoc->m_pkEncodingTool);
	m_wndProperties.AddProperty(lpkDoc->m_pkGenre);
	m_wndProperties.AddProperty(lpkDoc->m_pkGrouping);
	m_wndProperties.AddProperty(lpkDoc->m_pkName);
	m_wndProperties.AddProperty(lpkDoc->m_pkPartOfGaplessAlbum);
	m_wndProperties.AddProperty(lpkDoc->m_pkTempo);
	m_wndProperties.AddProperty(lpkDoc->m_pkTrack);
	m_wndProperties.AddProperty(lpkDoc->m_pkTotalTracks);

	CView::OnInitialUpdate();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CInformationView::OnDraw(CDC *ppkDC)
{
	// do nothing
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
LRESULT CInformationView::OnPropertyChanged(WPARAM plWParam, LPARAM plLParam)
{
	GetDocument()->SetModifiedFlag();
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CInformationView::OnSize(UINT piType, int piX, int piY)
{
	CView::OnSize(piType, piX, piY);

	if ( !IsWindow(m_wndProperties.GetSafeHwnd()) )
		return;

	// scale property grid
	m_wndProperties.MoveWindow(0, 0, piX, piY);
}
