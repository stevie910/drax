void DropData(HWND hwnd, IDataObject *pDataObject);

class CDropTarget : public IDropTarget
{
public:
	// IUnknown implementation
	HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
	ULONG   __stdcall AddRef(void);
	ULONG   __stdcall Release(void);

	// IDropTarget implementation
	HRESULT __stdcall DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	HRESULT __stdcall DragLeave(void);
	HRESULT __stdcall Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

	// Constructor
	CDropTarget(HWND hwnd);
	~CDropTarget();

private:
	// internal helper function
	bool  QueryDataObject(IDataObject *pDataObject);

	// Private member variables
	long   m_lRefCount;
	HWND   m_hWnd;
	bool   m_fAllowDrop;

	// Other internal window members

};