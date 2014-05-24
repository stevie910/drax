//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: Application.cpp
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Application.h"

#include "ChildFrame.h"
#include "MovieDoc.h"
#include "MovieView.h"
#include "MainFrame.h"

#include "InfoDlg.h"
#include "FormatDlg.h"
#include "DonationDlg.h"

// the application object
CApplication mkApp;

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CApplication::CApplication()
: m_pkDocTemplate(NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CApplication::~CApplication()
{
}

BEGIN_MESSAGE_MAP(CApplication, CWinAppEx)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_OPTIMIZE, OnFileOptimize)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPTIMIZE, OnUpdateFileOptimize)
	ON_COMMAND(ID_FILE_IMPORT_CHAPTERS, OnFileImportChapters)
	ON_COMMAND(ID_FILE_EXPORT_CHAPTERS, OnFileExportChapters)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_CHAPTERS, OnUpdateFileExportChapters)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_DONATE, OnHelpDonate)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CApplication::InitInstance()
{
	CWinApp::InitInstance();

	// set registry key
	SetRegistryKey(_T("drax"));
	CleanState();

	// create document template
	m_pkDocTemplate = new CMultiDocTemplate(
		IDR_MOVIE,
		RUNTIME_CLASS(CMovieDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CMovieView));
	AddDocTemplate(m_pkDocTemplate);

	// create main window
	CMainFrame *lpwndMainFrame = new CMainFrame();
	lpwndMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE);
	m_pMainWnd = lpwndMainFrame;
	lpwndMainFrame->ShowWindow(m_nCmdShow);
	lpwndMainFrame->UpdateWindow();

	// show donation dialog
	if ( !AfxIsDonated() )
	{
		CDonationDlg ldlgDonation(lpwndMainFrame);
		ldlgDonation.DoModal();
	}

	// parse command line
	return ParseCommandLine();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CDocument* CApplication::GetActiveDocument()
{
	// get main window
	CFrameWnd *lpwndMain = reinterpret_cast<CFrameWnd*>(m_pMainWnd);
	if ( !VALID_PTR(lpwndMain) )
		return NULL;

	// get frame window
	CFrameWnd *lpwndFrame = lpwndMain->GetActiveFrame();
	if ( !VALID_PTR(lpwndFrame) )
		return NULL;

	// get document
	return lpwndFrame->GetActiveDocument();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnUpdateFileSave(CCmdUI *ppkCmdUI)
{
	CDocument *lpkDoc = GetActiveDocument();
	ppkCmdUI->Enable(VALID_PTR(lpkDoc) && lpkDoc->IsModified());
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnFileOptimize()
{
	CString lsFilterExt;
	CString lsFilterName;

	// get document
	CDocument *lpkDoc = GetActiveDocument();
	ASSERT_VALID_PTR(lpkDoc);

	// get filter extension and name
	ASSERT_VALID_PTR(m_pkDocTemplate);
	m_pkDocTemplate->GetDocString(lsFilterExt, CDocTemplate::filterExt);
	m_pkDocTemplate->GetDocString(lsFilterName, CDocTemplate::filterName);

	lsFilterExt.Delete(0); // remove point in .M4V
	lsFilterName += _T("|*.") + lsFilterExt + _T("||"); // create filter

	// select file
	CFileDialog ldlgFile(FALSE, lsFilterExt, NULL, 4|2 , lsFilterName, GetMainWnd());
	if ( IDOK != ldlgFile.DoModal() )
		return;

	// DocumentFile = OptimizeFile?
	if ( 0 == lpkDoc->GetPathName().CompareNoCase(ldlgFile.GetPathName()) )
	{
		AfxMessageBox(IDS_ERROR_OPTIMIZE_EQUAL, MB_ICONERROR);
		return;
	}

	// warning
	AfxMessageBox(IDS_WARNING_OPTIMIZE, MB_ICONEXCLAMATION);

	// enable wait cursor
	CWaitCursor lcurWait;

	// optimize file
	if ( !MP4Optimize(CW2A(lpkDoc->GetPathName()), CW2A(ldlgFile.GetPathName())) )
		AfxMessageBox(IDS_ERROR_OPTIMIZE, MB_ICONERROR);
	else
		AfxMessageBox(IDS_INFO_OPTIMIZE, MB_ICONINFORMATION);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnUpdateFileOptimize(CCmdUI *ppkCmdUI)
{
	CDocument *lpkDoc = GetActiveDocument();
	ppkCmdUI->Enable(VALID_PTR(lpkDoc) && !lpkDoc->IsModified());
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnFileImportChapters()
{
	// select file
	CFileDialog ldlgFile(TRUE, _T("txt"), NULL, 4|2, AfxStringID(IDS_FILTER_TXT), GetMainWnd());
	if ( IDOK != ldlgFile.DoModal() )
		return;

	// select format
	CFormatDlg ldlgFormat(m_pMainWnd);
	if ( IDCANCEL == ldlgFormat.DoModal() )
		return;

	// ask the user
	if ( IDNO == AfxMessageBox(IDS_QUESTION_IMPORT_CHAPTERS, MB_ICONQUESTION|MB_YESNO) )
		return;

	// get current document
	CMovieDoc *lpkDoc = dynamic_cast<CMovieDoc*>(GetActiveDocument());
	ASSERT_VALID_PTR(lpkDoc);

	// import chapter
	if ( !lpkDoc->ImportChapters(ldlgFile.GetPathName(), ldlgFormat.GetFormat()) )
		AfxMessageBox(IDS_ERROR_IMPORT_CHAPTERS, MB_ICONERROR);
	else
		AfxMessageBox(IDS_INFO_IMPORT_CHAPTERS, MB_ICONINFORMATION);

	// update view
	lpkDoc->UpdateAllViews(NULL);
	lpkDoc->SetModifiedFlag();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnFileExportChapters()
{
	// select file
	CFileDialog ldlgFile(FALSE, _T("txt"), NULL, 4|2, AfxStringID(IDS_FILTER_TXT), GetMainWnd());
	if ( IDOK != ldlgFile.DoModal() )
		return;

	// select format
	CFormatDlg ldlgFormat(m_pMainWnd);
	if ( IDCANCEL == ldlgFormat.DoModal() )
		return;

	// get current document
	CMovieDoc *lpkDoc = dynamic_cast<CMovieDoc*>(GetActiveDocument());
	ASSERT_VALID_PTR(lpkDoc);
	
	// export chapter
	if ( !lpkDoc->ExportChapters(ldlgFile.GetPathName(), ldlgFormat.GetFormat()) )
		AfxMessageBox(IDS_ERROR_EXPORT_CHAPTERS, MB_ICONERROR);
	else
		AfxMessageBox(IDS_INFO_EXPORT_CHAPTERS, MB_ICONINFORMATION);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnUpdateFileExportChapters(CCmdUI *ppkCmdUI)
{
	CMovieDoc *lpkDoc = dynamic_cast<CMovieDoc*>(GetActiveDocument());
	ppkCmdUI->Enable(VALID_PTR(lpkDoc) && !lpkDoc->m_kChapters.IsEmpty());
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnAppAbout()
{
	CInfoDlg ldlgInfo(m_pMainWnd);
	ldlgInfo.DoModal();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CApplication::OnHelpDonate()
{
	ShellExecute(NULL, _T("OPEN"), DONATION_URL, NULL, NULL, SW_SHOW);
}


//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CApplication::ParseCommandLine()
{
	// parse command line
	CCmdLineParser lkCmdLineParser(GetCommandLine());

	// do command line arguments exist?
	if ( lkCmdLineParser.getVals().empty() )
		return TRUE; // show regular main window

	// find "file"
	if ( lkCmdLineParser.HasVal(_T("file")) )
		return ParseCommandLineForFileAction(lkCmdLineParser);

	// find "folder"
	if ( lkCmdLineParser.HasVal(_T("folder")) )
		return ParseCommandLineForFolderAction(lkCmdLineParser);

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CApplication::ParseCommandLineForFileAction(CCmdLineParser &pkCmdLineParser)
{
	CCmdLineParser_String lsValue;
	CMovieDoc *lpkDoc = NULL;
	BOOL lbDoSave = FALSE;

	// find "file"
	if ( !pkCmdLineParser.GetVal(_T("file"), lsValue) )
		return FALSE;

	// open "file"
	lpkDoc = dynamic_cast<CMovieDoc*>(OpenDocumentFile(lsValue));	
	if ( !VALID_PTR(lpkDoc) )
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// tags
	//////////////////////////////////////////////////////////////////////////////

	// Album
	if ( pkCmdLineParser.GetVal(_T("album"), lsValue) )
	{
		lpkDoc->m_pkAlbum->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}

	// Album Artist
	if ( pkCmdLineParser.GetVal(_T("album_artist"), lsValue) )
	{
		lpkDoc->m_pkAlbumArtist->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}

	// Artist
	if ( pkCmdLineParser.GetVal(_T("artist"), lsValue) )
	{
		lpkDoc->m_pkArtist->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}

	// Comment
	if ( pkCmdLineParser.GetVal(_T("comments"), lsValue) )
	{
		lpkDoc->m_pkComments->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}

	// Composer
	if ( pkCmdLineParser.GetVal(_T("composer"), lsValue) )
	{
		lpkDoc->m_pkComposer->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}
	
	// Compilation
	if ( pkCmdLineParser.GetVal(_T("compilation"), lsValue) )
	{
		lpkDoc->m_pkCompilation->SetValue(COleVariant((short)_wtoi(lsValue.GetString()), VT_BOOL));
		lbDoSave = TRUE;
	}

	// Disk
	if ( pkCmdLineParser.GetVal(_T("disk"), lsValue) )
	{
		lpkDoc->m_pkDisk->SetValue(COleVariant((long)_wtoi(lsValue.GetString()), VT_UI4));
		lbDoSave = TRUE;
	}

	// Total Disks
	if ( pkCmdLineParser.GetVal(_T("total_disks"), lsValue) )
	{
		lpkDoc->m_pkTotalDisks->SetValue(COleVariant((long)_wtoi(lsValue.GetString()), VT_UI4));
		lbDoSave = TRUE;
	}

	// Encoding Tool
	if ( pkCmdLineParser.GetVal(_T("encoding_tool"), lsValue) )
	{
		lpkDoc->m_pkEncodingTool->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}

	// Genre
	if ( pkCmdLineParser.GetVal(_T("genre"), lsValue) )
	{
		lpkDoc->m_pkGenre->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}
	
	// Grouping
	if ( pkCmdLineParser.GetVal(_T("grouping"), lsValue) )
	{
		lpkDoc->m_pkGrouping->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}
	
	// Name
	if ( pkCmdLineParser.GetVal(_T("name"), lsValue) )
	{
		lpkDoc->m_pkName->SetValue(COleVariant(lsValue));
		lbDoSave = TRUE;
	}
	
	// Part Of Gapless Album
	if ( pkCmdLineParser.GetVal(_T("part_of_gapless_album"), lsValue) )
	{
		lpkDoc->m_pkPartOfGaplessAlbum->SetValue(COleVariant((short)_wtoi(lsValue.GetString()), VT_BOOL));
		lbDoSave = TRUE;
	}
	
	// Tempo
	if ( pkCmdLineParser.GetVal(_T("tempo"), lsValue) )
	{
		lpkDoc->m_pkTempo->SetValue(COleVariant((long)_wtoi(lsValue.GetString()), VT_UI4));
		lbDoSave = TRUE;
	}
	
	// Track
	if ( pkCmdLineParser.GetVal(_T("track"), lsValue) )
	{
		lpkDoc->m_pkTrack->SetValue(COleVariant((long)_wtoi(lsValue.GetString()), VT_UI4));
		lbDoSave = TRUE;
	}
	
	// Total Tracks
	if ( pkCmdLineParser.GetVal(_T("total_tracks"), lsValue) )
	{
		lpkDoc->m_pkTotalTracks->SetValue(COleVariant((long)_wtoi(lsValue.GetString()), VT_UI4));
		lbDoSave = TRUE;
	}

	//////////////////////////////////////////////////////////////////////////////
	// chapter import & manipulation
	//////////////////////////////////////////////////////////////////////////////	
	
	// import
	if ( pkCmdLineParser.GetVal(_T("import"), lsValue) )
	{
		lpkDoc->ImportChapters(lsValue, 0);
		lbDoSave = TRUE;
	}
	
	// import2
	if ( pkCmdLineParser.GetVal(_T("import2"), lsValue) )
	{
		lpkDoc->ImportChapters(lsValue, 1);
		lbDoSave = TRUE;
	}

	// clear_chapters
	if ( pkCmdLineParser.GetVal(_T("clear_chapters"), lsValue) )
	{
		lbDoSave = lpkDoc->ClearChapters();
	}

	// autocreate_chapters
	if ( pkCmdLineParser.GetVal(_T("autocreate_chapters"), lsValue) )
	{
		lbDoSave = lpkDoc->AutoCreateChapters(_wtoi(lsValue.GetString()));
	}

	//////////////////////////////////////////////////////////////////////////////
	// SAVE
	//////////////////////////////////////////////////////////////////////////////	

	// save?
	if ( lbDoSave )
	{
		lpkDoc->DoFileSave();
		lbDoSave = FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////
	// chapter export
	//////////////////////////////////////////////////////////////////////////////	
	
	// export
	if ( pkCmdLineParser.GetVal(_T("export"), lsValue) )
	{
		lpkDoc->ExportChapters(lsValue, 0);
	}

	// export2
	if ( pkCmdLineParser.GetVal(_T("export2"), lsValue) )
	{
		lpkDoc->ExportChapters(lsValue, 1);
	}

	//////////////////////////////////////////////////////////////////////////////
	// various
	//////////////////////////////////////////////////////////////////////////////

	// optimize
	if ( pkCmdLineParser.GetVal(_T("optimize"), lsValue) )
	{
		if ( !MP4Optimize(CW2A(lpkDoc->GetPathName()), CW2A(lsValue)) )
			AfxMessageBox(IDS_ERROR_OPTIMIZE, MB_ICONERROR);		
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL CApplication::ParseCommandLineForFolderAction(CCmdLineParser &pkCmdLineParser)
{
	CCmdLineParser_String lsValue;
	CMovieDoc *lpkDoc = NULL;
	BOOL lbDoSave = FALSE;

	// find "folder"
	if ( !pkCmdLineParser.GetVal(_T("folder"), lsValue) )
		return FALSE;

	// don't accept empty strings
	if ( lsValue.IsEmpty() )
		return FALSE;

	// add missing backslash
	if ( _T('\\') != lsValue.GetAt(lsValue.GetLength() - 1) )
		lsValue += _T('\\');

	// add file pattern
	lsValue+= _T("*.*");

	// iterate files in folder
	CFileFind lkFileFind;
	BOOL lbWorking = lkFileFind.FindFile(lsValue);
	while ( lbWorking )
	{
		// get next file
		lbWorking = lkFileFind.FindNextFile();
		
		// ignore directories
		if ( lkFileFind.IsDirectory() )
			continue;

		// extract filename and file extension
		CString lsFileName = lkFileFind.GetFilePath();
		CString lsFileExt = lsFileName.Right(3);

		// m4v and mp4 files are valid
		if ( 0 == lsFileExt.CompareNoCase(_T("m4v")) ||
			 0 == lsFileExt.CompareNoCase(_T("mp4")) )
		{
			// open file
			lpkDoc = dynamic_cast<CMovieDoc*>(OpenDocumentFile(lsFileName));
			if ( !VALID_PTR(lpkDoc) )
				continue;

			//////////////////////////////////////////////////////////////////////////////
			// chapter manipulation
			//////////////////////////////////////////////////////////////////////////////	

			// clear_chapters
			if ( pkCmdLineParser.GetVal(_T("clear_chapters"), lsValue) )
			{
				lbDoSave = lpkDoc->ClearChapters();
			}

			//////////////////////////////////////////////////////////////////////////////
			// SAVE
			//////////////////////////////////////////////////////////////////////////////	

			// save?
			if ( lbDoSave )
			{
				lpkDoc->DoFileSave();
				lbDoSave = FALSE;
			}
		}
	}

	return FALSE;
}