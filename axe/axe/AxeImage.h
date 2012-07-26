///////////////////
// CaptureScreen.h
///////////////////
class CAxeImage
{  
	HANDLE            m_hDIB;
public:
	void    Capture();
	BOOL    WriteToDIB(CRect &rect, CSize &destSize, HANDLE &hDIB); // DIB에 쓰기
	HANDLE  DDBToDIB(CBitmap* pBitmap, DWORD dwCompression = BI_RGB, CPalette* pPal = NULL); // DDB 에서 DIB로 변환
};
