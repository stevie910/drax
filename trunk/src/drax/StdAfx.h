//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: StdAfx.h
// Date......: 02.12.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _AFX_ALL_WARNINGS

#include <afxwin.h>
#include <afxext.h>

#include <afxdisp.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

#include <afxcontrolbars.h>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
// includes
//////////////////////////////////////////////////////////////////////////////

#include <afxcview.h>
#include "resource.h"

//////////////////////////////////////////////////////////////////////////////
// helper
//////////////////////////////////////////////////////////////////////////////

CString AfxStringID(int piID);
CString AfxIntToString(int piValue);
int AfxStringToInt(const CString &psString);
CString AfxStringFromUtf8(const char *ppszString, int piLength = 0);
int AfxStringToUtf8(const CString &psString, char *&ppszString);

//////////////////////////////////////////////////////////////////////////////
// macros
//////////////////////////////////////////////////////////////////////////////

#define VALID_PTR(p)        (NULL != p)
#define ASSERT_VALID_PTR(p) ASSERT(VALID_PTR(p))
#define DELETE_S(p)         if (VALID_PTR(p)) delete [] p;

//////////////////////////////////////////////////////////////////////////////
// donation
//////////////////////////////////////////////////////////////////////////////

#define DONATION_URL    _T("https://www.paypal.com/donate/?hosted_button_id=P2KR838VXL4T6")
#define DONATION_TRIALS 3

BOOL AfxIsDonated();
void AfxSetDonated(BOOL pbDonated = DONATION_TRIALS);

#ifdef _UNICODE
#define ATOI _wtoi
#else
#define ATOI atoi
#endif

//////////////////////////////////////////////////////////////////////////////
// Debug
//////////////////////////////////////////////////////////////////////////////

/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/
