//////////////////////////////////////////////////////////////////////////////
// Project...: drax
// File......: Picture.cpp
// Date......: 22.11.2008
// Author....: Stephan Arlt
//////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Picture.h"

#define HIMETRIC_PER_INCH         2540
#define MAP_LOGHIM_TO_PIX(x,ppli) MulDiv((ppli), (x), HIMETRIC_PER_INCH)

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CPicture::CPicture(UINT piID, int piX, int piY)
: m_pkPicture(NULL),
  m_iX(piX),
  m_iY(piY)
{
	//////////////////////////////////////////////////////////////////////////////
	// find resource
	//////////////////////////////////////////////////////////////////////////////

	HRSRC lhRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(piID), _T("PICTURE"));
	ASSERT_VALID_PTR(lhRes);
	
	// load and lock resource
	HGLOBAL lhMem1 = LoadResource(GetModuleHandle(NULL), lhRes);
	ASSERT_VALID_PTR(lhMem1);	
	
	LPVOID lpvData1 = LockResource(lhMem1);
	ASSERT_VALID_PTR(lpvData1);
	
	DWORD ldwSize = SizeofResource(GetModuleHandle(NULL), lhRes);	
	ASSERT(0 != ldwSize);

	//////////////////////////////////////////////////////////////////////////////
	// copy resource
	//////////////////////////////////////////////////////////////////////////////

	HGLOBAL lhMem2 = GlobalAlloc(GMEM_MOVEABLE, ldwSize);
	ASSERT_VALID_PTR(lhMem2);
	
	LPVOID lpvData2 = GlobalLock(lhMem2);
	ASSERT_VALID_PTR(lpvData2);

	memcpy(lpvData2, lpvData1, ldwSize);
	VERIFY(0 == GlobalUnlock(lhMem2));

	//////////////////////////////////////////////////////////////////////////////
	// convert resource and load picture
	//////////////////////////////////////////////////////////////////////////////

	LPSTREAM lpStream = NULL;
	VERIFY(S_OK == CreateStreamOnHGlobal(lhMem2, TRUE, &lpStream));
	VERIFY(S_OK == OleLoadPicture(lpStream, 0, FALSE, IID_IPicture, (void**)&m_pkPicture));
	lpStream->Release();
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
CPicture::~CPicture()
{
	//////////////////////////////////////////////////////////////////////////////
	// release picture
	//////////////////////////////////////////////////////////////////////////////

	if ( VALID_PTR(m_pkPicture) )
	{
		m_pkPicture->Release();
		m_pkPicture = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
void CPicture::Paint(CDC *ppkDC)
{
	if ( !VALID_PTR(m_pkPicture) )
		return;

	// get width and height
	OLE_XSIZE_HIMETRIC *lpWidth = new OLE_XSIZE_HIMETRIC;
	OLE_YSIZE_HIMETRIC *lpHeight = new OLE_YSIZE_HIMETRIC;
	m_pkPicture->get_Width(lpWidth);
	m_pkPicture->get_Height(lpHeight);

	// convert width and height
	long llWidth = MAP_LOGHIM_TO_PIX(*lpWidth, ppkDC->GetDeviceCaps(LOGPIXELSX));
	long llHeight = MAP_LOGHIM_TO_PIX(*lpHeight, ppkDC->GetDeviceCaps(LOGPIXELSY));

	// render picture
	m_pkPicture->Render(ppkDC->GetSafeHdc(), m_iX, m_iY, llWidth, llHeight,
		0, *lpHeight, *lpWidth, -(*lpHeight), NULL);

	delete lpWidth;
	delete lpHeight;
}
