// http://www.catch22.net/tuts/drop-target


#include "stdafx.h"
#include <windows.h>
#include "Application.h"
#include "DropTarget.h"


CDropTarget::CDropTarget(HWND hwnd)
{
	m_lRefCount = 1;
	m_hWnd = hwnd;
	m_fAllowDrop = false;
}


CDropTarget::~CDropTarget()
{
}


//
//	IUnknown::QueryInterface
//
HRESULT __stdcall CDropTarget::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (iid == IID_IDropTarget || iid == IID_IUnknown)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}


//
//	IUnknown::AddRef
//
ULONG __stdcall CDropTarget::AddRef(void)
{
	return InterlockedIncrement(&m_lRefCount);
}


//
//	IUnknown::Release
//
ULONG __stdcall CDropTarget::Release(void)
{
	LONG count = InterlockedDecrement(&m_lRefCount);

	if (count == 0)
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}


//
//	IDropTarget::DragEnter
//
HRESULT __stdcall CDropTarget::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	m_fAllowDrop = QueryDataObject(pDataObject);

	*pdwEffect = m_fAllowDrop ? *pdwEffect & DROPEFFECT_COPY : DROPEFFECT_NONE;
	return S_OK;
}

//
//	IDropTarget::DragOver
//
//
//
HRESULT __stdcall CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	*pdwEffect = m_fAllowDrop ? *pdwEffect & DROPEFFECT_COPY : DROPEFFECT_NONE;
	return S_OK;
}

//
//	IDropTarget::DragLeave
//
HRESULT __stdcall CDropTarget::DragLeave(void)
{
	return S_OK;
}

//
//	IDropTarget::Drop
//
HRESULT __stdcall CDropTarget::Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if (m_fAllowDrop) DropData(m_hWnd, pDataObject);

	*pdwEffect = m_fAllowDrop ? *pdwEffect & DROPEFFECT_COPY : DROPEFFECT_NONE;
	return S_OK;
}

void DropData(HWND hwnd, IDataObject *pDataObject)
{
	FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (S_OK != pDataObject->QueryGetData(&fmtetc)) return;

	COleDataObject dataobj;
	dataobj.Attach(pDataObject, false);
	HGLOBAL hglobal = dataobj.GetGlobalData(CF_HDROP);
	if (NULL == hglobal) return;

	TCHAR szItem[MAX_PATH];
	TCHAR *szExt;
	HDROP hdrop = (HDROP)GlobalLock(hglobal);
	UINT uNumFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
	for (UINT uFile = 0; uFile < uNumFiles; uFile++) {
		if (0 != DragQueryFile(hdrop, uFile, szItem, MAX_PATH)) {
			szExt = PathFindExtension(szItem);
			if (0 == _tcscmp(L".mp4", szExt) || 0 == _tcscmp(L".m4v", szExt)) {
				AfxGetApp()->OpenDocumentFile(szItem);
			}
		}
	}
	GlobalUnlock(hglobal);
}


//
//	QueryDataObject private helper routine
//
bool CDropTarget::QueryDataObject(IDataObject *pDataObject)
{
	FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (S_OK != pDataObject->QueryGetData(&fmtetc)) return false;

	COleDataObject dataobj;
	dataobj.Attach(pDataObject, false);
	HGLOBAL hglobal = dataobj.GetGlobalData(CF_HDROP);
	if (NULL == hglobal) return false;

	bool bFoundMovie = false;
	TCHAR szItem[MAX_PATH];
	TCHAR *szExt;
	HDROP hdrop = (HDROP)GlobalLock(hglobal);
	UINT uNumFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
	for (UINT uFile = 0; uFile < uNumFiles; uFile++) {
		if (0 != DragQueryFile(hdrop, uFile, szItem, MAX_PATH)) {
			szExt = PathFindExtension(szItem);
			if (0 == _tcscmp(L".mp4", szExt) || 0 == _tcscmp(L".m4v", szExt)) {
				bFoundMovie = true;
				break;
			}
		}
	}
	GlobalUnlock(hglobal);

	return bFoundMovie;
}
