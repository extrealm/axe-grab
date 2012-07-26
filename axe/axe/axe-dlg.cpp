// axe-dlg.cpp : implementation file
//

#include "stdafx.h"
#include "axe.h"
#include "axe-dlg.h"
#include "PathString.h"
#include "notifyicon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAxeDlg dialog




CAxeDlg::CAxeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAxeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAxeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAxeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CAPTURE_REGION, &CAxeDlg::OnBnClickedCaptureRegion)
	ON_MESSAGE(WM_HOTKEY, OnHotkey)
	ON_MESSAGE(UM_TRAYICON, OnTrayMessage)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLIPBOARD_CONVERTSLASH, &CAxeDlg::OnClipboardConvertslash)
	ON_COMMAND(ID_SHELL_CYGWIN_PATH, &CAxeDlg::OnShellCygwinPath)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAxeDlg message handlers

BOOL CAxeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	BOOL b;
	b = RegisterHotKey(GetSafeHwnd(), 0x1004, MOD_CONTROL|MOD_SHIFT, VK_F11);
	if (!b) AfxMessageBox(_T("Registered failed"), MB_OK | MB_ICONEXCLAMATION);

	b = RegisterHotKey(GetSafeHwnd(), 0x1005, MOD_CONTROL|MOD_SHIFT, VK_F12);
	if (!b) AfxMessageBox(_T("Registered failed"), MB_OK | MB_ICONEXCLAMATION);

	AddNotifyIcon(GetSafeHwnd(), UM_TRAYICON, AfxGetApp()->LoadIcon(IDR_MAINFRAME), _T("AXE"), UM_TRAYICON);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAxeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAxeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAxeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "AxeCaptureWnd.h"
CAxeCaptureWnd* gCaptureWnd;

void CAxeDlg::OnBnClickedCaptureRegion()
{
	if (!gCaptureWnd)
	{
		gCaptureWnd = new CAxeCaptureWnd(NULL);
		gCaptureWnd->Create(CAxeCaptureWnd::IDD);
	}
	
	gCaptureWnd->ShowWindow(SW_SHOW);
#ifndef DEBUG
	gCaptureWnd->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	gCaptureWnd->ActivateTopParent();
	gCaptureWnd->SetFocus();
#endif
}

LRESULT CAxeDlg::OnHotkey(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0x1004:
		PostMessage(WM_COMMAND, IDC_CAPTURE_REGION);
		break;
	case 0x1005:
		PostMessage(WM_COMMAND, ID_CLIPBOARD_CONVERTSLASH);
		break;
	default:
		break;
	}

	return 0;
}
void CAxeDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here
	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);
	CMenu* p = menu.GetSubMenu(0);
	p->TrackPopupMenu(0, point.x, point.y, this);
}

void CAxeDlg::OnClipboardConvertslash()
{
	if ( !OpenClipboard() )
	{
		AfxMessageBox(_T("Cannot open the Clipboard"));
		return;
	}

	HANDLE h = ::GetClipboardData(CF_UNICODETEXT);
	if (!h)
		return;


	LPVOID p = GlobalLock(h);
	CString str((LPCWSTR)p);
	GlobalUnlock(h);

	if (str.Find(_T('\\')) >= 0) {
		str.Replace(_T('\\'), _T('/'));
	} else if (str.Find(_T('/')) >= 0) {
		str.Replace(_T('/'), _T('\\'));
	} else {
		CloseClipboard();
		return;
	}

	int cch = str.GetLength() + 1;
	int nBytes =  cch * sizeof(WCHAR);

	h = GlobalAlloc(GMEM_MOVEABLE, nBytes);
	p = GlobalLock(h);
	CopyMemory(p, (LPCWSTR)str, nBytes);
	GlobalUnlock(h);

	// Remove the current Clipboard contents  
	if( !EmptyClipboard() )
	{
		AfxMessageBox(_T("Cannot empty the Clipboard"));
		return;  
	}

	// For the appropriate data formats...
	if ( ::SetClipboardData( CF_UNICODETEXT, h ) == NULL )  
	{
		AfxMessageBox(_T("Unable to set Clipboard data"));    
		CloseClipboard();
		return;  
	}

	// ...  
	CloseClipboard();

}

void CAxeDlg::OnShellCygwinPath()
{
	if ( !OpenClipboard() )
	{
		AfxMessageBox(_T("Cannot open the Clipboard"));
		return;
	}

	HANDLE h = ::GetClipboardData(CF_UNICODETEXT);
	if (!h)
		return;


	LPVOID p = GlobalLock(h);
	CString str((LPCWSTR)p);
	GlobalUnlock(h);

	if (str.Find(_T('\\')) >= 0) {
		str.Replace(_T('\\'), _T('/'));
		str.Replace(_T(':'), _T('/'));
		str = CString(_T("/cygdrive/")) + str;
	} else if (str.Find(_T('/')) >= 0) {
		AfxMessageBox(_T("Converting Cygwin path to normal path is not supported yet"));
	} else {
		CloseClipboard();
		return;
	}

	int cch = str.GetLength() + 1;
	int nBytes =  cch * sizeof(WCHAR);

	h = GlobalAlloc(GMEM_MOVEABLE, nBytes);
	p = GlobalLock(h);
	CopyMemory(p, (LPCWSTR)str, nBytes);
	GlobalUnlock(h);

	// Remove the current Clipboard contents  
	if( !EmptyClipboard() )
	{
		AfxMessageBox(_T("Cannot empty the Clipboard"));
		return;  
	}

	// For the appropriate data formats...
	if ( ::SetClipboardData( CF_UNICODETEXT, h ) == NULL )  
	{
		AfxMessageBox(_T("Unable to set Clipboard data"));    
		CloseClipboard();
		return;  
	}

	// ...  
	CloseClipboard();
}

void CAxeDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);

	//CDialog::OnClose();
}


LRESULT CAxeDlg::OnTrayMessage(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONUP:
		{
			CMenu menu, *pMenu;
			CPoint pt;

			menu.LoadMenu(IDR_CONTEXT);
			pMenu = menu.GetSubMenu(0);
			GetCursorPos(&pt);
			pMenu->TrackPopupMenu(TPM_RIGHTALIGN,pt.x,pt.y,this);
		}
		break;
	
		case WM_LBUTTONDBLCLK:
		{
			ShowWindow(SW_SHOW);
		};
		break;

		default:
			return 0;
	}

	return TRUE;
}

void CAxeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	DeleteTaskBarIcon(this->GetSafeHwnd(), UM_TRAYICON);
}
