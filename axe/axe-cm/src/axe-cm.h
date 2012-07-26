//---------------------------------------------------------------------------
// Copyright 2002-2003 by Andre Burgaud <andre@burgaud.com>
// See license.txt
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// wscitecm.h
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Modified by Don HO <don.h@free.fr>
// to meet the need of Notepad++
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Modified by Austin Shin <extrealm@naver.com>
// to meet the need of AXE
//---------------------------------------------------------------------------

// {5CC9D36D-1515-4c22-ACB6-056593AA4043}
DEFINE_GUID(CLSID_ShellExtension, 0x5cc9d36d, 0x1515, 0x4c22, 0xac, 0xb6, 0x5, 0x65, 0x93, 0xaa, 0x40, 0x43);


class CShellExtClassFactory : public IClassFactory {
protected:
  ULONG m_cRef;

public:
  CShellExtClassFactory();
  ~CShellExtClassFactory();

  STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();
  STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
  STDMETHODIMP LockServer(BOOL);
};
typedef CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;

class CShellExt : public IContextMenu, IShellExtInit {
public:
protected:
  ULONG m_cRef;
  UINT m_cbFiles;
  STGMEDIUM m_stgMedium;
  LPDATAOBJECT m_pDataObj;
  HBITMAP m_hAxeBmp;
  LPMALLOC m_pAlloc;
  TCHAR m_szDllDir [MAX_PATH];

  STDMETHODIMP InvokeAxe(HWND hParent,
    LPCSTR pszWorkingDir,
    LPCSTR pszCmd,
    LPCSTR pszParam,
    int iShowCmd);

public:
  CShellExt();
  ~CShellExt();

  STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();

  STDMETHODIMP QueryContextMenu(HMENU hMenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags);

  STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);

  STDMETHODIMP GetCommandString(UINT idCmd,
    UINT uFlags,
    UINT FAR *reserved,
    LPSTR pszName,
    UINT cchMax);

  STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder,
    LPDATAOBJECT pDataObj,
    HKEY hKeyID);
};

typedef CShellExt *LPCSHELLEXT;
