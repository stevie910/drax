//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: StdAfx.cpp
// Date......: 02.12.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CString AfxStringID(int piID)
{
	CString lsString;
	VERIFY(lsString.LoadString(piID));
	return lsString;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CString AfxIntToString(int piValue)
{
	CString lsString;
	lsString.Format(_T("%d"), piValue);
	return lsString;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
int AfxStringToInt(const CString &psString)
{
	return ATOI(psString);
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CString AfxStringFromUtf8(const char *ppszString, int piLength)
{
	if ( !VALID_PTR(ppszString) )
		return _T(""); // no error

	// get string length
	int liLength = (0 == piLength) ? strlen(ppszString) : piLength;
	liLength = MultiByteToWideChar(CP_UTF8, 0, ppszString, liLength, NULL, NULL);

	// alloc and convert string
	CString lsString;
	LPTSTR lpszString = lsString.GetBuffer(liLength + 1);
	MultiByteToWideChar(CP_UTF8, 0, ppszString, -1, lpszString, liLength);
	lpszString[liLength] = _T('\0');
	lsString.ReleaseBuffer();
	
	return lsString;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
int AfxStringToUtf8(const CString &psString, char *&ppszString)
{
	if ( 0 == psString.GetLength() )
	{
		ppszString = NULL;
		return 0; // no error
	}

	// get string length
	int liLength = WideCharToMultiByte(CP_UTF8, 0, psString, psString.GetLength(), NULL, NULL, NULL, NULL);

	// alloc and convert string
	ppszString = new char[liLength + 1];
	WideCharToMultiByte(CP_UTF8, 0, psString, -1, ppszString, liLength, NULL, NULL);
	ppszString[liLength] = '\0';

	return liLength;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
BOOL AfxIsDonated()
{
	// get donation trials
	int liDonationTrials = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DonationTrials"), 0);

	// eval donation trials
	if ( liDonationTrials < DONATION_TRIALS )
	{
		AfxSetDonated(liDonationTrials + 1);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void AfxSetDonated(int piDonationTrials)
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DonationTrials"), piDonationTrials);
}