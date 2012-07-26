#pragma once


// CAxeCaptureWnd dialog

class CAxeCaptureWnd : public CDialog
{
	DECLARE_DYNAMIC(CAxeCaptureWnd)

public:
	CAxeCaptureWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAxeCaptureWnd();

// Dialog Data
	enum { IDD = IDD_AXE_CAPTURE };
public:
	CBitmap	m_bmScreen;
	float _scale;
	int _ox, _oy, _cx, _cy;
	enum { ePan, eMarker, ePen, eText } m_nTool;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int CaptureScreen(int mode);
	int CaptureRedraw(CDC* pDC);
	int CaptureZoomTo(LPPOINT pt);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClose();
protected:
	virtual void OnCancel();
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureSave();
};
