//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: MovieDoc.cpp
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MovieDoc.h"

/////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CMovieDoc::CMovieDoc()
: m_hReferenceTrackID(0),
  m_hChapterTrackID(0),
  m_hDuration(0)
{
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CMovieDoc::~CMovieDoc()
{
}

IMPLEMENT_DYNCREATE(CMovieDoc, CDocument)

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::OnOpenDocument(LPCTSTR ppszPathName)
{
	// open file
	MP4FileHandle lhFile = MP4Read(CW2A(ppszPathName));
	if ( MP4_INVALID_FILE_HANDLE == lhFile )
	{
		AfxMessageBox(IDS_ERROR_FILE, MB_ICONERROR);
		return FALSE;
	}

	// load information
	if ( !LoadInformation(lhFile) )
		return FALSE;

	// load chapters
	if ( !LoadChapters(lhFile) )
		return FALSE;

	// close file
	MP4Close(lhFile);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::OnSaveDocument(LPCTSTR ppszPathName)
{
	// open file
	MP4FileHandle lhFile = MP4Modify(CW2A(ppszPathName));
	if ( MP4_INVALID_FILE_HANDLE == lhFile )
	{
		AfxMessageBox(IDS_ERROR_FILE, MB_ICONERROR);
		return FALSE;
	}

	// store information
	if ( !StoreInformation(lhFile) )
		return FALSE;

	// store chapters
	if ( !StoreChapters(lhFile) )
		return FALSE;

	// close file
	MP4Close(lhFile);
	SetModifiedFlag(FALSE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::LoadInformation(MP4FileHandle phFile)
{
	// alloc and fetch tags
	const MP4Tags *lptTags = MP4TagsAlloc();
	MP4TagsFetch(lptTags, phFile);

	// Album
	m_pkAlbum = new CMFCPropertyGridProperty(_T("Album"),
		COleVariant(AfxStringFromUtf8(lptTags->album), VT_BSTR));

	// Album Artist
	m_pkAlbumArtist = new CMFCPropertyGridProperty(_T("Album Artist"),
		COleVariant(AfxStringFromUtf8(lptTags->albumArtist), VT_BSTR));

	// Artist
	m_pkArtist = new CMFCPropertyGridProperty(_T("Artist"),
		COleVariant(AfxStringFromUtf8(lptTags->artist), VT_BSTR));

	// Comments
	m_pkComments = new CMFCPropertyGridProperty(_T("Comments"),
		COleVariant(AfxStringFromUtf8(lptTags->comments), VT_BSTR));

	// Compilation
	m_pkCompilation = new CMFCPropertyGridProperty(_T("Compilation"),
		COleVariant((!VALID_PTR(lptTags->compilation)) ? (short)0 : (short)*lptTags->compilation, VT_BOOL));

	// Composer
	m_pkComposer = new CMFCPropertyGridProperty(_T("Composer"),
		COleVariant(AfxStringFromUtf8(lptTags->composer), VT_BSTR));	

	// Disk
	m_pkDisk = new CMFCPropertyGridProperty(_T("Disk"),
		COleVariant((!VALID_PTR(lptTags->disk)) ? (long)0 : (long)lptTags->disk->index, VT_UI4));

	// Total Disks
	m_pkTotalDisks = new CMFCPropertyGridProperty(_T("Total Disks"),
		COleVariant((!VALID_PTR(lptTags->disk)) ? (long)0 : (long)lptTags->disk->total, VT_UI4));

	// Encoding Tool
	m_pkEncodingTool = new CMFCPropertyGridProperty(_T("Encoding Tool"),
		COleVariant(AfxStringFromUtf8(lptTags->encodingTool), VT_BSTR));
	
	// Genre
	m_pkGenre = new CMFCPropertyGridProperty(_T("Genre"),
		COleVariant(AfxStringFromUtf8(lptTags->genre), VT_BSTR));

	// Grouping
	m_pkGrouping = new CMFCPropertyGridProperty(_T("Grouping"),
		COleVariant(AfxStringFromUtf8(lptTags->grouping), VT_BSTR));

	// Name
	m_pkName = new CMFCPropertyGridProperty(_T("Name"),
		COleVariant(AfxStringFromUtf8(lptTags->name), VT_BSTR));

	// Part Of Gapless Album
	m_pkPartOfGaplessAlbum = new CMFCPropertyGridProperty(_T("Part Of Gapless Album"),
		COleVariant((!VALID_PTR(lptTags->gapless)) ? (short)0 : (short)*lptTags->gapless, VT_BOOL));

	// Tempo
	m_pkTempo = new CMFCPropertyGridProperty(_T("Tempo"),
		COleVariant((!VALID_PTR(lptTags->tempo)) ? (long)0 : (long)*lptTags->tempo, VT_UI4));

	// Track
	m_pkTrack = new CMFCPropertyGridProperty(_T("Track"),
		COleVariant((!VALID_PTR(lptTags->track)) ? (long) 0 : (long)lptTags->track->index, VT_UI4));

	// Total Tracks
	m_pkTotalTracks = new CMFCPropertyGridProperty(_T("Total Tracks"),
		COleVariant((!VALID_PTR(lptTags->track)) ? (long)0 : (long)lptTags->track->total, VT_UI4));

	// clean up
	MP4TagsFree(lptTags);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::LoadChapters(MP4FileHandle phFile)
{
	// find video track
	m_hReferenceTrackID = MP4FindTrackId(phFile, 0, MP4_VIDEO_TRACK_TYPE, 0);
	if ( MP4_INVALID_TRACK_ID == m_hReferenceTrackID )
	{
		// find audio track
		m_hReferenceTrackID = MP4FindTrackId(phFile, 0, MP4_AUDIO_TRACK_TYPE, 0);
		if ( MP4_INVALID_TRACK_ID == m_hReferenceTrackID )
		{
			AfxMessageBox(IDS_ERROR_VIDEO_TRACK, MB_ICONERROR);
			return FALSE;
		}
	}

	// get and convert duration
	MP4Duration lhDuration = MP4GetTrackDuration(phFile, m_hReferenceTrackID);
	m_hDuration = MP4ConvertFromTrackDuration(phFile, m_hReferenceTrackID, lhDuration, MP4_MILLISECONDS_TIME_SCALE);
	
	// load chapter track?
	m_hChapterTrackID = MP4FindTrackId(phFile, 0, MP4_TEXT_TRACK_TYPE, 0);
	if ( MP4_INVALID_TRACK_ID == m_hChapterTrackID )
		return TRUE; // no error

	// get chapter count
	int liChapters = MP4GetTrackNumberOfSamples(phFile, m_hChapterTrackID);
	if ( 0 == liChapters )
	{
		AfxMessageBox(IDS_ERROR_CHAPTERS, MB_ICONERROR);
		return FALSE;
	}

	// get chapters
	for ( int i = 1; i <= liChapters; i++ )
	{
		// timestamp
		MP4Timestamp lhTimestamp;
		int liTimestamp;

		// chapter name
		uint32_t liSampleCount = 1024;
		uint8_t *lpszSample = new uint8_t[liSampleCount];

		// get timestamp
		if ( !MP4ReadSample(phFile, m_hChapterTrackID, i, &lpszSample, &liSampleCount, &lhTimestamp) )
		{
			AfxMessageBox(IDS_ERROR_CHAPTER, MB_ICONEXCLAMATION);
			delete [] lpszSample; // clean up
			continue;
		}

		// convert timestamp
		liTimestamp = (int)MP4ConvertFromTrackTimestamp(phFile, m_hChapterTrackID, lhTimestamp, MP4_MILLISECONDS_TIME_SCALE);
		if ( MP4_INVALID_TIMESTAMP == liTimestamp )
		{
			AfxMessageBox(IDS_ERROR_TIMESTAMP, MB_ICONEXCLAMATION);
			delete [] lpszSample; // clean up
			continue;
		}

		// get and convert chapter name
		int liNameCount = (lpszSample[0] << 8) | lpszSample[1];
		CString lsName = AfxStringFromUtf8((char*)&lpszSample[2], liNameCount);

		// init chapter
		CChapter *lpkChapter = new CChapter();
		lpkChapter->m_sName = lsName;
		lpkChapter->m_kTime.FromMilliseconds(liTimestamp);
		m_kChapters.Add(lpkChapter);

		// clean up
		delete [] lpszSample;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::StoreInformation(MP4FileHandle phFile)
{
	// helper vars
	uint8_t lbMetadata;
	uint16_t liMetadata;
	char *lpszMetadata;

	// alloc and fetch tags
	const MP4Tags *lptTags = MP4TagsAlloc();
	MP4TagsFetch(lptTags, phFile);

	// Album
	AfxStringToUtf8(m_pkAlbum->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetAlbum(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);

	// Album Artist
	AfxStringToUtf8(m_pkAlbumArtist->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetAlbumArtist(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);

	// Artist
	AfxStringToUtf8(m_pkArtist->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetArtist(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);

	// Comments
	AfxStringToUtf8(m_pkComments->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetComments(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);

	// Compilation
	lbMetadata = (uint8_t)m_pkCompilation->GetValue().iVal;
	MP4TagsSetCompilation(lptTags, !lbMetadata ? NULL : &lbMetadata);

	// Composer
	AfxStringToUtf8(m_pkComposer->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetComposer(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);

	// Disk / Total Disks
	MP4TagDisk ltDisk;
	ltDisk.index = (uint16_t)m_pkDisk->GetValue().lVal;
	ltDisk.total = (uint16_t)m_pkTotalDisks->GetValue().lVal;

	if ( 0 == ltDisk.index && 0 == ltDisk.total )
		MP4TagsSetDisk(lptTags, NULL);
	else
		MP4TagsSetDisk(lptTags, &ltDisk);	

	// Encoding Tool
	AfxStringToUtf8(m_pkEncodingTool->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetEncodingTool(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);
	
	// Genre
	AfxStringToUtf8(m_pkGenre->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetGenre(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);
	
	// Grouping
	AfxStringToUtf8(m_pkGrouping->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetGrouping(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);
	
	// Name
	AfxStringToUtf8(m_pkName->GetValue().bstrVal, lpszMetadata);
	MP4TagsSetName(lptTags, lpszMetadata);
	DELETE_S(lpszMetadata);
	
	// Part Of Gapless Album
	lbMetadata = (uint8_t)m_pkPartOfGaplessAlbum->GetValue().iVal;
	MP4TagsSetGapless(lptTags, !lbMetadata ? NULL : &lbMetadata);

	// Tempo
	liMetadata = (uint16_t)m_pkTempo->GetValue().lVal;
	MP4TagsSetTempo(lptTags, (0 == liMetadata) ? NULL : &liMetadata);
	
	// Track / Total Tracks
	MP4TagTrack ltTrack;
	ltTrack.index = (uint16_t)m_pkTrack->GetValue().lVal;
	ltTrack.total = (uint16_t)m_pkTotalTracks->GetValue().lVal;
	
	if ( 0 == ltTrack.index && 0 == ltTrack.total )
		MP4TagsSetTrack(lptTags, NULL);
	else
		MP4TagsSetTrack(lptTags, &ltTrack);

	// store and clean up
	MP4TagsStore(lptTags, phFile);
	MP4TagsFree(lptTags);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::StoreChapters(MP4FileHandle phFile)
{
	// delete old chapter track
	if ( m_hChapterTrackID > 0 )
	{
		MP4DeleteTrack(phFile, m_hChapterTrackID);
		m_hChapterTrackID = 0;
	}

	// store new chapter track?
	if ( m_kChapters.IsEmpty() )
		 return TRUE; // no error
	
	// find position for new chapter track
	for ( int i = 0; i < 1024; i++ )
	{
		MP4TrackId lhChapterTrack = MP4FindTrackId(phFile, i);
		if ( MP4_INVALID_TRACK_ID == lhChapterTrack )
		{
			MP4SetIntegerProperty(phFile, "moov.mvhd.nextTrackId", i + 1);
			break;
		}
	}

	// add new chapter track
	m_hChapterTrackID = MP4AddChapterTextTrack(phFile, m_hReferenceTrackID);
	if ( MP4_INVALID_TRACK_ID == m_hChapterTrackID )
	{
		AfxMessageBox(IDS_ERROR_CHAPTER_TRACK, MB_ICONERROR);
		return FALSE;
	}

	// write chapters
	for ( int i = 0; i < m_kChapters.GetSize(); i++ )
	{
		CChapter *lpkChapter = m_kChapters[i];
		ASSERT_VALID_PTR(lpkChapter);

		// get and convert chapter name
		char *lpszName = NULL; 
		int liNameCount = AfxStringToUtf8(lpkChapter->m_sName, lpszName);

		// create sample
		uint32_t liSampleCount = liNameCount + 2 + 12;
		uint8_t *lpszSample = new uint8_t[liSampleCount];
						
		// 2-byte length marker
		lpszSample[0] = (liNameCount >> 8) & 0xFF;
		lpszSample[1] = liNameCount & 0xFF;

		// chapter name
		strncpy((char*)&lpszSample[2], lpszName, liNameCount);
		int liOffset = liNameCount + 2;
		
		// modifier length marker
		lpszSample[liOffset] = 0x00;
		lpszSample[liOffset + 1] = 0x00;
		lpszSample[liOffset + 2] = 0x00;
		lpszSample[liOffset + 3] = 0x0C;
		
		// modifier type code
		lpszSample[liOffset + 4] = 'e';
		lpszSample[liOffset + 5] = 'n';
		lpszSample[liOffset + 6] = 'c';
		lpszSample[liOffset + 7] = 'd';
		
		// modifier value
		lpszSample[liOffset + 8] = 0x00;
		lpszSample[liOffset + 9] = 0x00;
		lpszSample[liOffset + 10] = (256 >> 8) & 0xFF;
		lpszSample[liOffset + 11] = 256 & 0xFF;

		// get duration
		MP4Duration lhDuration;
		if ( (i + 1) == m_kChapters.GetSize() )
			lhDuration = m_hDuration - m_kChapters[i]->m_kTime.ToMilliseconds();
		else
			lhDuration = m_kChapters[i + 1]->m_kTime.ToMilliseconds() - m_kChapters[i]->m_kTime.ToMilliseconds();

		// convert duration
		MP4Duration lhTrackDuration = MP4ConvertToTrackDuration(phFile, m_hChapterTrackID, lhDuration, MP4_MILLISECONDS_TIME_SCALE);
		if ( MP4_INVALID_DURATION == lhTrackDuration )
		{
			AfxMessageBox(IDS_ERROR_DURATION, MB_ICONEXCLAMATION);
			delete [] lpszName;
			delete [] lpszSample;
			continue;
		}

		// write chapter
		if ( !MP4WriteSample(phFile, m_hChapterTrackID, lpszSample, liSampleCount, lhTrackDuration) )
		{
			AfxMessageBox(IDS_ERROR_CHAPTER, MB_ICONEXCLAMATION);
			delete [] lpszName;
			delete [] lpszSample;
			continue;
		}

		// clean up
		delete [] lpszName;
		delete [] lpszSample;		
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::AddEditChapter(int piChapter, CTimeSpanEx &pkTime, const CString &psName)
{
	// when adding the first chapter, time must be 0!
	if ( m_kChapters.IsEmpty() && !(0 == pkTime.ToMilliseconds()) )
	{
		AfxMessageBox(IDS_ERROR_CHAPTER_MODIFY_FIRST_ADD, MB_ICONERROR);
		return FALSE;
	}

	// when editing the first chapter, time must be 0!
	if ( 0 == piChapter && !(0 == pkTime.ToMilliseconds()) )
	{
		AfxMessageBox(IDS_ERROR_CHAPTER_MODIFY_FIRST_EDIT, MB_ICONERROR);
		return FALSE;
	}

	// the time of the chapter must be less than the time of the movie!
	if ( !(pkTime.ToMilliseconds() < m_hDuration)  )
	{
		AfxMessageBox(IDS_ERROR_CHAPTER_MODIFY_DURATION_FULL, MB_ICONERROR);
		return FALSE;
	}

	// the name of the chapter must not be empty!
	if ( psName.IsEmpty() )
	{
		AfxMessageBox(IDS_ERROR_CHAPTER_MODIFY_NAME_EMPTY, MB_ICONERROR);
		return FALSE;
	}

	// the time and the name of the chapter must not be in the list!
	int liChapter = 0;
	for ( int i = 0; i < m_kChapters.GetSize(); i++ )
	{
		if ( i == piChapter )
			continue; // skip current chapter

		// get chapter
		CChapter *lpkChapter = m_kChapters[i];
		ASSERT_VALID_PTR(lpkChapter);

		// time
		if ( lpkChapter->m_kTime.ToMilliseconds() == pkTime.ToMilliseconds() )
		{
			AfxMessageBox(IDS_ERROR_CHAPTER_MODIFY_TIME, MB_ICONERROR);
			return FALSE;
		}

		// name
		if ( lpkChapter->m_sName == psName )
		{
			AfxMessageBox(IDS_ERROR_CHAPTER_MODIFY_NAME, MB_ICONERROR);
			return FALSE;
		}

		// chapter index
		if ( pkTime.ToMilliseconds() > lpkChapter->m_kTime.ToMilliseconds() )
			liChapter = i + 1;
	}

	if ( piChapter >= 0 )
	{
		// remove chapter
		m_kChapters.RemoveAt(piChapter);
		if ( liChapter > piChapter )
		{
			// consider removed chapter
			liChapter--;
		}
	}

	// init and add chapter
	CChapter *lpkChapter = new CChapter();
	lpkChapter->m_kTime = pkTime;
	lpkChapter->m_sName = psName;
	m_kChapters.InsertAt(liChapter, lpkChapter);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::AddEditChapter(int piChapter, const CString &psTime, const CString &psName)
{
	// convert string to time
	CTimeSpanEx lkTime(psTime);
	return AddEditChapter(piChapter, lkTime, psName);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::RemoveChapter(int piChapter)
{
	// when the first chapter is supposed to be removed, only 1 element must be in the list!
	if ( 0 == piChapter && 1 != m_kChapters.GetSize() )
	{
		AfxMessageBox(IDS_ERROR_CHAPTER_MODIFY_FIRST_REMOVE, MB_ICONERROR);
		return FALSE;
	}

	// remove chapter
	m_kChapters.RemoveAt(piChapter);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::ClearChapters()
{
	// remove all chapters
	m_kChapters.RemoveAll();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::AutoCreateChapters(int piSeconds)
{
	// no negativ seconds and no 0
	if ( piSeconds <= 0 )
		return FALSE;

	// remove all chapters
	ClearChapters();

	// create chapters automatically
	for ( int i = 0, j = 0; j < m_hDuration; i++, j += (piSeconds * 1000) )
	{
		// time
		CTimeSpanEx lkTime;
		lkTime.FromMilliseconds(j);

		// name
		CString lsName;
		lsName.Format(_T("Chapter %d"), i + 1);

		// add chapter
		if ( !AddEditChapter(-1, lkTime, lsName) )
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::ImportChapters(LPCTSTR ppszPathName, int piFormat)
{
	BOOL lbResult = FALSE;

	try
	{
		// remove existing chapters
		m_kChapters.RemoveAll();

		// open file
		FILE *lptFile = _tfopen(ppszPathName, _T("r, ccs=UTF-8"));
		CStdioFile lkFile(lptFile);

		// read chapters
		CString lsLine; CStringArray lkLines;
		while ( lkFile.ReadString(lsLine) )
			lkLines.Add(lsLine);

		// close file
		lkFile.Close();

		// select format
		switch ( piFormat )
		{
		case 0:
			lbResult = ImportChaptersFormat1(lkLines); // format 1
			break;

		case 1:
			lbResult = ImportChaptersFormat2(lkLines); // format 2
			break;

		default:
			ASSERT(FALSE); // error
			break;
		}
	}
	catch ( CFileException *e )
	{
		e->ReportError();
		e->Delete();
	}

	return lbResult;
}

//////////////////////////////////////////////////////////////////////////////
// 00:00:00.000 Chapter 1
// ...
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::ImportChaptersFormat1(const CStringArray &pkLines)
{
	int liChapter = 0;

	// ignore first line (for Sony Vegas?)
	if ( pkLines.GetSize() > 0 && 0 == pkLines[0].Compare(_T("Position\tName")) )
	{
		liChapter = 1;
	}

	while ( liChapter < pkLines.GetSize() )
	{
		// get line
		CString lsLine = pkLines[liChapter];

		// get time and name
		CString lsTime = lsLine.Mid(0, 12);
		CString lsName = lsLine.Mid(13);
		
		// add chapter
		if ( !AddEditChapter(-1, lsTime, lsName) ) {
			return FALSE;
		}

		liChapter++; // next chapter
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// CHAPTER01=00:00:00.000
// CHAPTER01NAME=Chapter 1
// ...
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::ImportChaptersFormat2(const CStringArray &pkLines)
{
	for ( int i = 0; (i + 1) < pkLines.GetSize(); i += 2 )
	{
		// get lines
		CString lsLineTime = pkLines[i];
		CString lsLineName = pkLines[i + 1];

		// get position separator
		int liPosTime = lsLineTime.Find(_T('='));
		int liPosName = lsLineName.Find(_T('='));

		// validation
		if ( -1 == liPosTime || -1 == liPosName )
			continue;

		// get time and name
		CString lsTime = lsLineTime.Right(lsLineTime.GetLength() - liPosTime - 1);
		CString lsName = lsLineName.Right(lsLineName.GetLength() - liPosName - 1);

		// add chapter
		if ( !AddEditChapter(-1, lsTime, lsName) ) {
			return FALSE;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::ExportChapters(LPCTSTR ppszPathName, int piFormat)
{
	BOOL lbResult = FALSE;

	try
	{
		// select format
		switch ( piFormat )
		{
		case 0:
			lbResult = ExportChaptersFormat1(ppszPathName); // format 1
			break;

		case 1:
			lbResult = ExportChaptersFormat2(ppszPathName); // format 2
			break;

		default:
			ASSERT(FALSE); // error
			break;
		}
	}
	catch ( CFileException *e )
	{
		e->ReportError();
		e->Delete();
	}

	return lbResult;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::ExportChaptersFormat1(LPCTSTR ppszPathName)
{
	try
	{
		// open file
		FILE *lptFile = _tfopen(ppszPathName, _T("w, ccs=UTF-8"));
		CStdioFile lkFile(lptFile);

		for ( int i = 0; i < m_kChapters.GetSize(); i++ )
		{
			CChapter *lpkChapter = m_kChapters[i];
			ASSERT_VALID_PTR(lpkChapter);

			// format and write chapter
			CString lsChapter;
			lsChapter.Format(_T("%s %s\n"), lpkChapter->m_kTime.ToString(), lpkChapter->m_sName);
			lkFile.WriteString(lsChapter);
		}

		// close file
		lkFile.Close();
	}
	catch ( CFileException *e )
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CMovieDoc::ExportChaptersFormat2(LPCTSTR ppszPathName)
{
	try
	{
		// open file
		FILE *lptFile = _tfopen(ppszPathName, _T("w, ccs=UTF-8"));
		CStdioFile lkFile(lptFile);

		for ( int i = 0; i < m_kChapters.GetSize(); i++ )
		{
			CChapter *lpkChapter = m_kChapters[i];
			ASSERT_VALID_PTR(lpkChapter);

			// format and write chapter
			CString lsChapter;
			lsChapter.Format(_T("CHAPTER%02d=%s\nCHAPTER%02dNAME=%s\n"), (i + 1), lpkChapter->m_kTime.ToString(), (i + 1), lpkChapter->m_sName);
			lkFile.WriteString(lsChapter);
		}

		// close file
		lkFile.Close();
	}
	catch ( CFileException *e )
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return TRUE;
}
