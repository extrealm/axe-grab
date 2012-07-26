// AxeCaptureWnd.cpp : implementation file
//

#include "stdafx.h"
#include "axe.h"
#include "AxeCaptureWnd.h"
#include "AxeImage.h"
#include "jpeg.h"

// CAxeCaptureWnd dialog

IMPLEMENT_DYNAMIC(CAxeCaptureWnd, CDialog)

CAxeCaptureWnd::CAxeCaptureWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CAxeCaptureWnd::IDD, pParent)
	, m_nTool(ePan), _scale(1.5), _ox(0), _oy(0), _cx(0), _cy(0)
{

}

CAxeCaptureWnd::~CAxeCaptureWnd()
{
}

void CAxeCaptureWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAxeCaptureWnd, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CAPTURE_SAVE, &CAxeCaptureWnd::OnCaptureSave)
END_MESSAGE_MAP()


// CAxeCaptureWnd message handlers

int CAxeCaptureWnd::CaptureScreen(int mode)
{
	// TODO: Add your control notification handler code here
	_cx = ::GetSystemMetrics(SM_CXSCREEN);
	_cy = ::GetSystemMetrics(SM_CYSCREEN);

	HDC sdc = ::GetDC(NULL);
	HDC mdc = ::CreateCompatibleDC(sdc);
	HBITMAP hbm = ::CreateCompatibleBitmap(sdc, _cx, _cy);
	HGDIOBJ obj = ::SelectObject(mdc, hbm);
	::BitBlt(mdc, 0, 0, _cx, _cy, sdc, 0, 0, SRCCOPY);

	if (m_bmScreen.GetSafeHandle())
		m_bmScreen.DeleteObject();

	m_bmScreen.Attach(hbm);

	::SelectObject(mdc, obj);

	::DeleteDC(mdc);
	::ReleaseDC(NULL, sdc);

	return 0;
}

int CAxeCaptureWnd::CaptureZoomTo(LPPOINT p)
{
	// no translation now.

	return 0;
}

int CAxeCaptureWnd::CaptureRedraw(CDC* pDC)
{
	if (!m_bmScreen.GetSafeHandle())
		return -1;

	int x, y, w, h;

	// scroll to center position
	w = (_cx / _scale);
	h = (_cy / _scale);
	x = _ox - w / 2;
	y = _oy - h / 2;

	// limit scroll
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x + w > _cx) x = _cx - w;
	if (y + h > _cy) y = _cy - h;

	// draw
	HDC sdc, mdc;
	if (pDC) sdc = pDC->GetSafeHdc();
	else sdc = ::GetDC(NULL);
	mdc = ::CreateCompatibleDC(sdc);
	HGDIOBJ obj = ::SelectObject(mdc, m_bmScreen.GetSafeHandle());
	::StretchBlt(sdc, 0, 0, _cx, _cy, mdc, x, y, w, h, SRCCOPY);
	::SelectObject(mdc, obj);
	::DeleteDC(mdc);
	if (pDC)
		::ReleaseDC(NULL, sdc);

	return 0;
}

// int CAxeCaptureWnd::CaptureDPtoLP(LPPOINT p)
// {
// 	w = (_cx / _scale);
// 	h = (_cy / _scale);
// 	x = _ox - w / 2;
// 	y = _oy - h / 2;
// 
// 	p->x = x + p->x / _scale;
// 	p->y = y + p->y / _scale;
// }

void CAxeCaptureWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	int cx = ::GetSystemMetrics(SM_CXSCREEN);
	int cy = ::GetSystemMetrics(SM_CYSCREEN);

	CaptureScreen(0);
	_ox = cx / 2; 
	_oy = cy / 2;
	SetWindowPos(&wndTop, 0, 0, cx, cy, SWP_NOREDRAW);
	ModifyStyleEx(0, WS_EX_TOPMOST);
	if (bShow)
	{
		SetCapture();
	}
	else
	{
		ReleaseCapture();
		return;
	}

	CDialog::OnShowWindow(bShow, nStatus);
}

void CAxeCaptureWnd::OnPaint()
{
	CPaintDC dc(this);

	CaptureRedraw(&dc);
}

BOOL CAxeCaptureWnd::OnEraseBkgnd(CDC* pDC)
{
	// do not erase at all.
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CAxeCaptureWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CPoint p = pt;
	CaptureZoomTo(&p);

	float speed = 1.2 * (zDelta / WHEEL_DELTA);
	if (speed > 0)	_scale = _scale * speed;
	else			_scale = _scale / (-speed);

	if (_scale < 1) _scale = 1;
	if (_scale > 10) _scale = 10;

	Invalidate();

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CAxeCaptureWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint p = point;
	CaptureZoomTo(&p);

	switch (m_nTool)
	{
	case ePan:
		_ox = p.x;
		_oy = p.y;

		Invalidate();
		break;

	case eMarker:
		break;
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CAxeCaptureWnd::OnClose()
{
	ShowWindow(SW_HIDE);
	// CDialog::OnClose();
}

void CAxeCaptureWnd::OnCancel()
{
	ShowWindow(SW_HIDE);

	// CDialog::OnCancel();
}

void CAxeCaptureWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here
	CMenu m;
	m.LoadMenu(IDR_CONTEXT);
	CMenu* p = m.GetSubMenu(1);

	p->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CAxeCaptureWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonDown(nFlags, point);
}

void CAxeCaptureWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonUp(nFlags, point);
}

void CAxeCaptureWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnRButtonDown(nFlags, point);
}

void CAxeCaptureWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnRButtonUp(nFlags, point);
}

#define ReturnAxeError0(s) { AfxMessageBox(s, MB_OK); return; }
#define ReturnAxeError1(s, e) { AfxMessageBox(s, MB_OK); return e; }

void CAxeCaptureWnd::OnCaptureSave()
{
	CFileDialog dlg(FALSE);
	if (dlg.DoModal() != IDOK)
		return;

	CString szFile = dlg.GetPathName();
	CAxeImage img;
	HANDLE hDib = img.DDBToDIB(&m_bmScreen, BI_RGB, NULL);
	if (!hDib)
		ReturnAxeError0(_T("Cannot get DIB"));

	CJpeg jpg;
	if (!jpg.LoadMemDIB(hDib))
		ReturnAxeError0(_T("Cannot get DIB"));

	if (!jpg.SaveJPG(szFile))
		ReturnAxeError0(_T("Cannot get DIB"));
}
