// axe-dlg.h : header file
//

#pragma once


// CAxeDlg dialog
class CAxeDlg : public CDialog
{
// Construction
public:
	CAxeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AXE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCaptureRegion();
	afx_msg LRESULT CAxeDlg::OnHotkey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT CAxeDlg::OnTrayMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnClipboardConvertslash();
	afx_msg void OnShellCygwinPath();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};
