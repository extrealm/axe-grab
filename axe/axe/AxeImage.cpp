#include "stdafx.h"

#include "AxeImage.h"
#include "jpeg.h"

///////////////////
// CaptureScreen.cpp
///////////////////
void CAxeImage::Capture()
{
	int      cx,cy;
	// ��ü ȭ�� ������
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);
	
	
	CRect m_rect(0, 0, cx, cy);
	CSize destSize(cx, cy);
	if(m_hDIB) {
		GlobalFree(m_hDIB);
	}
	WriteToDIB(m_rect, destSize, m_hDIB);
	CJpeg jpg;
	//jpg.LoadMemDIB(m_bmpinfo, (BYTE*)m_hDIB, cx*cy*3);
	jpg.SaveJPG(_T("C:\\capture.jpg"));
	///////////////////////
	// server�� ����
	///////////////////////
	// �������� ���� ����
	// ::DeleteFile("C:\\Capture.jpg");
	
}
BOOL CAxeImage::WriteToDIB(CRect &rect, CSize &destSize, HANDLE &hDIB)
{
	CDC      memDC;
	CDC      ScreenDC;
	CBitmap  m_Bitmap;
	
	UCHAR t;
	UCHAR temp[17*3];
	UCHAR *tt;
	UINT  m_width, h;
	UINT  m_height;
	m_width = rect.Width();
	m_height = rect.Height();
	//��ũ�� DC�� ��´�.
	ScreenDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	
	//��ũ�� DC�� ȣȯ�Ǵ� DC �� �����
	memDC.CreateCompatibleDC(&ScreenDC);
	//��ũ�� DC�� ȣȯ�Ǵ� ��Ʈ���� �����.
	m_Bitmap.CreateCompatibleBitmap(&ScreenDC, destSize.cx, destSize.cy);
	// ��Ʈ�� DC�� �����Ѵ�.
	CBitmap* pOldBitmap = memDC.SelectObject(&m_Bitmap);
	
	// ĸ���ؿ� ȭ���� m_Bitmap�� Copy
	// memDC.StretchBlt(0,0,cx,cy,&ScreenDC,0,0,cx,cy,SRCCOPY);
	memDC.BitBlt(0, 0, destSize.cx, destSize.cy, &ScreenDC, 0, 0, SRCCOPY);
	CPalette m_Pal;
	//���� �ػ󵵰� 256Į�� ���϶�� �ȷ�Ʈ �߰�
	if(ScreenDC.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;
		pLP->palNumEntries = GetSystemPaletteEntries(ScreenDC, 0, 255, pLP->palPalEntry);
		// Create the palette
		m_Pal.CreatePalette( pLP );
		delete[] pLP;
	}
	
	memDC.SelectObject(pOldBitmap);
	// Convert the bitmap to a DIB
	// DDB(Device-Dependent Bitmap)�� ĸ�ĵ� ȭ���� DIB(Device-Independent Bitmap)�� ��ȯ
	// HANDLE �� ����
	m_hDIB = DDBToDIB(&m_Bitmap, BI_RGB, &m_Pal);
	ScreenDC.DeleteDC();
	if (hDIB == NULL) {
		// AfxMessageBox("bitmap�� DIB���� convert�� �� �����ϴ�.", MB_ICONSTOP);
		return FALSE;
	}
	// DIB�� �ٲ� �޷θ� ����
	//   GlobalFree(hDIB);
	
	tt = new UCHAR[m_width * 3];
	for(h = 1; h < (UINT)m_height; h++)
	{
		memcpy(temp, (UCHAR*)hDIB + (h * (m_width * 3)), 13 * 3);
		memcpy(tt, (UCHAR*)hDIB + (h * (m_width * 3)), m_width * 3);
		memcpy((UCHAR*)hDIB + ((h) * (m_width * 3)), tt + (13 * 3), m_width * 3 - (13 * 3) );
		memcpy((UCHAR*)hDIB + ((h - 1) * (m_width * 3)) + ((m_width * 3) - 13 * 3), temp, 13 * 3);
	}
	memcpy(temp, (UCHAR*)hDIB, 13 * 3);
	memcpy(tt, (UCHAR*)hDIB, m_width * 3);
	memcpy((UCHAR*)hDIB, tt + (13 * 3), m_width * 3 - (13 * 3) );
	delete tt;
	
	for( h = 0 ; h < m_width * m_height * 3 ; h+=3)
	{
		memcpy(&t,((UCHAR*)hDIB+h+1), 1);
		memcpy(((UCHAR*)hDIB+h+1), ((UCHAR*)hDIB+h+2), 1);
		memcpy(((UCHAR*)hDIB+h+2), &t, 1);
		
		memcpy(&t, ((UCHAR*)hDIB+h), 1);
		memcpy(((UCHAR*)hDIB+h), ((UCHAR*)hDIB+h+2), 1);
		memcpy(((UCHAR*)hDIB+h+2), &t, 1);
	}
	return TRUE;
}
// DDB(Device-Dependent Bitmap)�� ĸ�ĵ� ȭ���� DIB(Device-Independent Bitmap)�� ��ȯ
HANDLE CAxeImage::DDBToDIB(CBitmap* pBitmap, DWORD dwCompression, CPalette *pPal)
{
	BITMAP       bm;
	// BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER  lpbi;
	BITMAPINFO        m_bmpinfo;
	DWORD       dwLen;
	HANDLE       hDIB;
	HANDLE        handle;
	HDC        hDC;
	HPALETTE      hPal;

	// Return Value : A HANDLE to the attached Windows GDI object
	if (!pBitmap || !pBitmap->GetSafeHandle())
		return NULL;

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal == NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	pBitmap->GetBitmap(&bm);

	// Initialize the bitmapinfoheader ( Bitmap Information )
	m_bmpinfo.bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);             // ��Ʈ�� ���ũ��
	m_bmpinfo.bmiHeader.biWidth      = bm.bmWidth;                           // ��Ʈ���� ���� ũ��
	m_bmpinfo.bmiHeader.biHeight   = bm.bmHeight;                          // ��Ʈ���� ���� ũ��
	m_bmpinfo.bmiHeader.biPlanes   = 1;                                    // Plane �� (1�� ����)
	m_bmpinfo.bmiHeader.biBitCount  = 24;                                   // �� �ȼ��� ��Ʈ��
	m_bmpinfo.bmiHeader.biCompression = dwCompression;                        // ���� ����
	m_bmpinfo.bmiHeader.biSizeImage  = bm.bmWidth * bm.bmHeight *3;          // �׸� ������ ũ��
	m_bmpinfo.bmiHeader.biXPelsPerMeter = 0;                                    // �� �ȼ��� ���� ����
	m_bmpinfo.bmiHeader.biYPelsPerMeter = 0;                                    // �� �ȼ��� ���� ����
	m_bmpinfo.bmiHeader.biClrUsed  = 0;                                    // �׸����� ���� ���Ǵ� �÷���
	m_bmpinfo.bmiHeader.biClrImportant = 0;                                    // �߿��ϰ� ���Ǵ� �÷�

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << m_bmpinfo.bmiHeader.biBitCount);
	if( nColors > 256 )
		nColors = 0;
	dwLen  = m_bmpinfo.bmiHeader.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);
	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	CopyMemory(lpbi, &m_bmpinfo.bmiHeader, sizeof(BITMAPINFOHEADER));

	// Call GetDIBits with a NULL lpBits param, so the device driver
	// will calculate the biSizeImage field
	GetDIBits(hDC,
		(HBITMAP)pBitmap->GetSafeHandle(),
		0L,
		(DWORD)m_bmpinfo.bmiHeader.biHeight,
		(LPBYTE)NULL,
		(LPBITMAPINFO)lpbi,
		(DWORD)DIB_RGB_COLORS);
	m_bmpinfo.bmiHeader = *lpbi;
	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (m_bmpinfo.bmiHeader.biSizeImage == 0)
	{
		m_bmpinfo.bmiHeader.biSizeImage =
		( ( ( (m_bmpinfo.bmiHeader.biWidth * m_bmpinfo.bmiHeader.biBitCount)  + 31)
		& ~31) / 8) * m_bmpinfo.bmiHeader.biHeight;
		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
		m_bmpinfo.bmiHeader.biSizeImage = (m_bmpinfo.bmiHeader.biSizeImage * 3);  //   / 2 �ƴϸ� �����
	}
	// Realloc the buffer so that it can hold all the bits
	dwLen += m_bmpinfo.bmiHeader.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);
		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}
	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	
	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)pBitmap->GetSafeHandle(),
		0L,    // Start scan line
		(DWORD)m_bmpinfo.bmiHeader.biHeight,  // # of scan lines
		(LPBYTE)lpbi    // address for bitmap bits
		+ (m_bmpinfo.bmiHeader.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,  // address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);  // Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	::ReleaseDC(NULL,hDC);
	// size = dwLen;
	return hDIB;
}
