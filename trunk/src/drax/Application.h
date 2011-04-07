//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: Application.h
// Date......: 21.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#pragma once

class CApplication : public CWinAppEx
{
public:
	CApplication();
	virtual ~CApplication();

	//////////////////////////////////////////////////////////////////////////////
	// overridings
	//////////////////////////////////////////////////////////////////////////////

	BOOL InitInstance();

	//////////////////////////////////////////////////////////////////////////////
	// getter
	//////////////////////////////////////////////////////////////////////////////

	CDocument* GetActiveDocument();

protected:
	CDocTemplate *m_pkDocTemplate;

protected:
	afx_msg void OnUpdateFileSave(CCmdUI *ppkCmdUI);
	afx_msg void OnFileOptimize();
	afx_msg void OnUpdateFileOptimize(CCmdUI *ppkCmdUI);
	afx_msg void OnFileImportChapters();
	afx_msg void OnFileExportChapters();
	afx_msg void OnUpdateFileExportChapters(CCmdUI *ppkCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnHelpDonate();
	BOOL ParseCommandLine();
	DECLARE_MESSAGE_MAP()
};