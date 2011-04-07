//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: MovieDoc.h
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <mp4v2\mp4v2.h>

#include "Chapter.h"
#include "TypedPtrArrayEx.hpp"

typedef CTypedPtrArrayEx<CChapter*> CChapterArray;

class CMovieDoc : public CDocument
{
public: // ignore information hiding :(
	MP4TrackId m_hReferenceTrackID;
	MP4TrackId m_hChapterTrackID;
	MP4Duration m_hDuration;
	CChapterArray m_kChapters;
	CMFCPropertyGridProperty *m_pkAlbum;
	CMFCPropertyGridProperty *m_pkAlbumArtist;
	CMFCPropertyGridProperty *m_pkArtist;
	CMFCPropertyGridProperty *m_pkComments;
	CMFCPropertyGridProperty *m_pkCompilation;
	CMFCPropertyGridProperty *m_pkComposer;
	CMFCPropertyGridProperty *m_pkDisk;
	CMFCPropertyGridProperty *m_pkTotalDisks;
	CMFCPropertyGridProperty *m_pkEncodingTool;
	CMFCPropertyGridProperty *m_pkGenre;
	CMFCPropertyGridProperty *m_pkGrouping;
	CMFCPropertyGridProperty *m_pkName;
	CMFCPropertyGridProperty *m_pkPartOfGaplessAlbum;
	CMFCPropertyGridProperty *m_pkTempo;
	CMFCPropertyGridProperty *m_pkTrack;
	CMFCPropertyGridProperty *m_pkTotalTracks;

public:
	CMovieDoc();
	virtual ~CMovieDoc();
	DECLARE_DYNCREATE(CMovieDoc)

	//////////////////////////////////////////////////////////////////////////////
	// overridings
	//////////////////////////////////////////////////////////////////////////////

	BOOL OnOpenDocument(LPCTSTR ppszPathName);
	BOOL OnSaveDocument(LPCTSTR ppszPathName);

	//////////////////////////////////////////////////////////////////////////////
	// chapters
	//////////////////////////////////////////////////////////////////////////////

	BOOL AddEditChapter(int piChapter, CTimeSpanEx &pkTime, const CString &psName);
	BOOL AddEditChapter(int piChapter, const CString &psTime, const CString &psName);
	BOOL RemoveChapter(int piChapter);

	BOOL ClearChapters();
	BOOL AutoCreateChapters(int piSeconds);

	//////////////////////////////////////////////////////////////////////////////
	// import / export
	//////////////////////////////////////////////////////////////////////////////

	BOOL ImportChapters(LPCTSTR ppszPathName, int piFormat);
	BOOL ExportChapters(LPCTSTR ppszPathName, int piFormat);

protected:
	//////////////////////////////////////////////////////////////////////////////
	// helper
	//////////////////////////////////////////////////////////////////////////////

	BOOL LoadInformation(MP4FileHandle phFile);
	BOOL LoadChapters(MP4FileHandle phFile);
	BOOL StoreInformation(MP4FileHandle phFile);
	BOOL StoreChapters(MP4FileHandle phFile);
	BOOL ImportChaptersFormat1(const CStringArray &pkLines);
	BOOL ImportChaptersFormat2(const CStringArray &pkLines);
	BOOL ExportChaptersFormat1(LPCTSTR ppszPathName);
	BOOL ExportChaptersFormat2(LPCTSTR ppszPathName);
};
