#include "stdafx.h"
#include "jpeg.h"

/* this is some i/o wrapper for file access. */
#include <io.h>
#include <fcntl.h>

inline int _jpgOpen(LPCTSTR fname)
{
	return _topen(fname, _O_CREAT | _O_TRUNC | _O_BINARY);
}

inline int _jpgClose(int fd)
{
	return _close(fd);
}

inline int _jpgWrite(int fd, void* buf, int cnt)
{
	return _write(fd, buf, cnt);
}

///////////////////
// Jpeg.cpp
///////////////////
CJpeg::CJpeg()
{
	m_pData = NULL;
	for(int i = 0; i < 20; i++)
		TbH[i].Flag = FALSE;
}
CJpeg::~CJpeg()
{
	if(m_pData != NULL)
		delete [] m_pData;

	for(int i = 0; i < 20; i++)
	{
		if(TbH[i].Flag)
		{
			delete [] TbH[i].HUFFCODE;
			delete [] TbH[i].HUFFSIZE;
			delete [] TbH[i].HUFFVAL;
		}
	}

	if(m_pDib)
		delete [] m_pDib;
}

// DIB데이터를 받아서 파일에 저장할 수 있도록 필요한 체이터를 저장해 놓는 루틴
BOOL CJpeg::LoadMemDIB(HANDLE hDIB)
{
	BITMAPINFOHEADER *p;

	p = (BITMAPINFOHEADER*)GlobalLock(hDIB);
	if (!p)
		return FALSE;

	memcpy(&m_pBitmapInfo, p, sizeof(BITMAPINFOHEADER));
	int size = m_pBitmapInfo.bmiHeader.biSizeImage;
	int colors = m_pBitmapInfo.bmiHeader.biClrUsed;
	BYTE* pPixels = ((BYTE*)p) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * colors);


	// 실제 픽셀 데이터 부분을 저장할 메모리 공간을 할당받고 pMemDIB를 저장합니다.
	m_pDib = new char [size];
	memcpy(m_pDib, pPixels, size);
	GlobalUnlock(hDIB);

	m_imageSize = size;

	return TRUE;
}
BOOL CJpeg::SaveJPG(LPCTSTR FileName, int Width, int Height, BYTE *pp)
{
	m_rWidth = Width;
	m_rHeight = Height;
	int BMPWidth = (Width * 3 + 3) / 4 * 4; // BITMAP의 4의 배수인 넓이
	int bWidth = Width;
	int bHeight = Height;
	if(Width % 8 != 0)
		bWidth = (Width/8 + 1)*8; // bWidth 는 8의 배수로 만든 넓이
	if(Height % 8 != 0)
		bHeight = (Height/8 + 1)*8; // bHeight 는 8의 배수로 만든 높이
	int i, j, k;

	// Huffman Table 초기화 //
	for(i=0; i<20; i++)
	{
		if(TbH[i].Flag)
		{
			delete [] TbH[i].HUFFCODE;
			delete [] TbH[i].HUFFSIZE;
			delete [] TbH[i].HUFFVAL;
		}
	}
	HFILE hFile = _jpgOpen(FileName);
	if (hFile < 0)
		return FALSE;

	PutSOI(hFile);      // Start Of Image //
	PutDQT(hFile);      // Quantization Table 저장 //
	PutDHT(hFile);      // Huffman Table 저장 //
	PutSOF(hFile, Width, Height);  // FrameHeader 저장 //
	PutSOS(hFile);      // Scan Header 저장 //
	// 버퍼를 마련합시다! //
	if(m_pData != NULL)
		delete [] m_pData;
	m_pData = new BYTE[(bWidth * 3) * bHeight];
	memset(m_pData, 0, (bWidth * 3) * bHeight); // 0으로 초기화 //
	// 8의 배수에 맞도록 버퍼를 만들어서 원래 이미지를 복사합니다 //
	for(i=0; i<Height; i++)
		memcpy(&m_pData[i*(bWidth*3)], &pp[i*BMPWidth], BMPWidth);
	// RGB Color를 YCbCr Color로 변환합니다. //
	float R, G, B;
	float y, cb, cr;
	BYTE *pos;
	for(i=0; i<Height; i++)
	{
		pos = &m_pData[i*(bWidth*3)];
		for(j=0; j<Width; j++)
		{
			B = (float)*pos;
			G = (float)*(pos+1);
			R = (float)*(pos+2);
			y = 0.299f * R + 0.587f * G + 0.114f * B;
			cb = -0.1687f * R - 0.3313f * G + 0.5f * B + 128;
			cr = 0.5f*R - 0.4187f*G - 0.0813f*B + 128;
			// y = (77 * R + 150 * G + 29 * B)>>8;
			// cb = (B - y) + 128;
			// cr = (R - y) + 128;
			if(y>255.) y = 255.; if(y<0.) y = 0.;
			if(cb>255.) cb = 255.; if(cb<0.) cb = 0.;
			if(cr>255.) cr = 255.; if(cr<0.) cr = 0.;
			*pos = (BYTE)y;
			*(pos+1) = (BYTE)cb;
			*(pos+2) = (BYTE)cr;
			pos = pos + 3;
		}
	}
	// Y, Cb, Cr Plane Buffer //
	Y = new short[bWidth * bHeight];
	memset(Y, 0, sizeof(short) * bWidth * bHeight);
	Cb = new short[bWidth * bHeight];
	memset(Cb, 0, sizeof(short) * bWidth * bHeight);
	Cr = new short[bWidth * bHeight];
	memset(Cr, 0, sizeof(short) * bWidth * bHeight);
	int idx1, idx2;
	// Level Shifting -128 //
	for(i=0; i<Height; i++)
	{
		for(j=0; j<Width; j++)
		{
			idx1 = i*bWidth+j;
			idx2 = i*(bWidth*3)+j*3;
			Y[idx1] = m_pData[idx2] - 128;
			Cb[idx1] = m_pData[idx2+1] - 128;
			Cr[idx1] = m_pData[idx2+2] - 128;
		}
	}
	// DCT & Zigzag & Quantization //
	for(i=0; i<bHeight; i = i + 8)
	{
		for(j=0; j<bWidth; j = j + 8)
		{
			DCT(&Y[i*bWidth+j], bWidth, FALSE);
			DCT(&Cb[i*bWidth+j], bWidth, TRUE);
			DCT(&Cr[i*bWidth+j], bWidth, TRUE);
		}
	}
	// Differential DC Process //
	int Num_X = bWidth/8;
	int Num_Y = bHeight/8;
	int idx = 0;
	short *DC1 = new short[Num_X * Num_Y];
	short *DC2 = new short[Num_X * Num_Y];
	short *DC3 = new short[Num_X * Num_Y];
	for(i=0; i<bHeight; i = i + 8)
	{
		for(j=0; j<bWidth; j = j + 8)
		{
			DC1[idx] = Y[i*bWidth+j];
			DC2[idx] = Cb[i*bWidth+j];
			DC3[idx] = Cr[i*bWidth+j];
			idx++;
		}
	}
	for(i=Num_X * Num_Y - 1; i>0; i--)
	{
		DC1[i] = DC1[i] - DC1[i-1];
		DC2[i] = DC2[i] - DC2[i-1];
		DC3[i] = DC3[i] - DC3[i-1];
	}
	idx = 0;
	for(i=0; i<bHeight; i = i + 8)
	{
		for(j=0; j<bWidth; j = j + 8)
		{
			Y[i*bWidth+j] = DC1[idx];
			Cb[i*bWidth+j] = DC2[idx];
			Cr[i*bWidth+j] = DC3[idx];
			idx++;
		}
	}
	delete [] DC1;
	delete [] DC2;
	delete [] DC3;

	/* AC 허프만 테이블 검색을 빠르게 하기 위하여
	허프만 테이블 구조체에 있는 *PT를 설정한다. */
	int Num, iTh[4] = {16, 17}, Th, key;
	for(i=0; i<2; i++)
	{
		Th = iTh[i];
		Num = TbH[Th].Num;
		TbH[Th].PT = new int[251];
		for(k=0; k<251; k++)
		{
			key = -9999;
			for(j=0; j<Num; j++)
			{
				if(TbH[Th].HUFFVAL[j] == k)
				{
					key = j;
					break;
				}
			}
			TbH[Th].PT[k] = key;
		}
	}

	/* HUFFMAN CODE ENCODING!! */
	m_pBuf = new BYTE[bWidth * bHeight * 3]; // 실제로 인코딩된 데이터가 저장될 버퍼
	m_Index = 0; // 인덱스 리셋
	cnt = 0; // Bit총 리셋
	hEncode(bWidth, bHeight);
	while(cnt != 0)
		ShotBit(0);
	_jpgWrite(hFile, (LPSTR)m_pBuf, m_Index);
	delete [] m_pBuf;
	PutEOI(hFile);

	// 임시 메모리 모두 해제 //
	for(i=0; i<2; i++)
	{
		Th = iTh[i];
		delete [] TbH[Th].PT;
	}
	for(i=0; i<bHeight; i++)
	{
		for(j=0; j<bWidth; j++)
		{
			idx1 = i*bWidth*3 + j*3;
			m_pData[idx1] = (BYTE)Y[i*bWidth+j];
			m_pData[idx1+1] = (BYTE)Cb[i*bWidth+j];
			m_pData[idx1+2] = (BYTE)Cr[i*bWidth+j];
		}
	}

	delete [] Y;
	delete [] Cb;
	delete [] Cr;

	// 파일 폐쇄 //
	_jpgClose(hFile);

	return 0;
}
// 지정한 파일로 m_pDib의 내용을 압축하기 위해 준비하는 루틴
BOOL CJpeg::SaveJPG(LPCTSTR path)
{
	// DIB 데이터는 Y 축이 거꾸로 되어 있으므로 맨 왼쪽윗 부분이 시작위치가 되도록
	// m_pDib의 내용을 바꿉니다.
	FlipY();
	//지정된 파일로 m_pDib을 저장하는 실제루틴을 호출합니다.
	SaveJPG(path, GetSize().cx, GetSize().cy, (BYTE*)m_pDib );
	return TRUE;
}
// DIB 데이터의 거꾸로 된 이미지를 좌측상단이 위로 가도록 바이트 오더를 재 정렬하는 루틴
void CJpeg::FlipY()
{
	// 한 스캔라인에 소요된 실제 바이트수 4의 배수를 구한다.
	int RealWidth = (GetSize().cx * 3 + 3) / 4 * 4;
	// 이미지의 높이를 구한다.
	int Height = GetSize().cy;
	//Flip을 하기 위해서 필요한 임지저장 장소에 메모리를 할당합니다.
	BYTE *pLine = new BYTE[RealWidth * Height];
	for( int i=0; i<Height/2; i++)
	{
		//윗부분의 한 스캔라인을 임시저장 장소에 카피고,
		memcpy(pLine, &m_pDib[i * RealWidth], RealWidth);
		//밑부분의 한 스캔라인을 윗부분의 스캔라인 위치고 카피하고,
		memcpy(&m_pDib[i * RealWidth], &m_pDib[(Height - 1 - i) * RealWidth], RealWidth);
		//임시저장장소에 있던 내용을 밑부분의 한 스캔라인으로 카피합니다.
		memcpy(&m_pDib[(Height - 1 - i) * RealWidth], pLine, RealWidth);
	}
	//임시 저장장소를 해제합니다.
	delete [] pLine;
}
//이미지의 가로크기와 세로크기를 얻어내는 루틴입니다.
SIZE CJpeg::GetSize()
{
	//BITMAPINFO 구조체의 bmiHeader 멤버의 biWidth멤버는 이미지의 가로크기
	//biHeight 멤버는 이미지의 세로크기입니다.
	SIZE size;
	size.cx=m_pBitmapInfo.bmiHeader.biWidth;
	size.cy=m_pBitmapInfo.bmiHeader.biHeight;
	return size;
}
void CJpeg::PutSOI(HFILE hFile)
{
	// SOI 저장 //
	WORD Marker = (0xd8 << 8) | 0xff;
	_jpgWrite(hFile, (LPSTR)&Marker, 2);
}
void CJpeg::PutDQT(HFILE hFile)
{
	WORD Marker, SegSize;
	BYTE c;
	// Luminance Quantization Table //
	BYTE Qtb0[64] = {16, 11, 12, 14, 12, 10, 16, 14,
		13, 14, 18, 17, 16, 19, 24, 40,
		26, 24, 22, 22, 24, 49, 36, 37,
		29, 40, 58, 51, 61, 60, 57, 51,
		56, 55, 64, 72, 92, 78, 64, 68,
		87, 69, 66, 57, 80, 109, 81, 87,
		95, 98, 103, 104, 103, 62, 77, 113,
		121, 112, 100, 120, 92, 101, 103, 99};
	// Chrominance Quantization Table //
	BYTE Qtb1[64] = {17, 18, 18, 24, 21, 24, 47, 26,
		26, 47, 99, 66, 56, 66, 99, 99,
		99 ,99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99 };
	Marker = (0xdb << 8) | 0xff;
	_jpgWrite(hFile, (LPSTR)&Marker, 2); // Marker
	SegSize = 67; SegSize = (SegSize << 8) | (SegSize >> 8);
	_jpgWrite(hFile, (LPSTR)&SegSize, 2); // Segment Size
	c = 0; _jpgWrite(hFile, (LPSTR)&c, 1); // PqTq = 0
	_jpgWrite(hFile, (LPSTR)Qtb0, 64); // Q0 ~ Q63
	Marker = (0xdb << 8) | 0xff;
	_jpgWrite(hFile, (LPSTR)&Marker, 2); // Marker
	SegSize = 67; SegSize = (SegSize << 8) | (SegSize >> 8);
	_jpgWrite(hFile, (LPSTR)&SegSize, 2); // Segment Size
	c = 1; _jpgWrite(hFile, (LPSTR)&c, 1); // PqTq = 1
	_jpgWrite(hFile, (LPSTR)Qtb1, 64); // Q0 ~ Q63
}
void CJpeg::PutDHT(HFILE hFile)
{
	/* standard 허프만 테이블을 읽어들여서 멤버 구조체에 설정한
	후 저장하고자 하는 파일에 적어넣는 함수입니다. */
	m_pBuf = new BYTE[421];
	BYTE HuffTb[421] = {255, 196, 1, 162, 0, 0, 1, 5, 1, 1, 1,
		1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2,
		3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 0, 2,
		1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1,
		125, 1, 2, 3, 0, 4, 17, 5, 18, 33, 49,
		65, 6, 19, 81, 97, 7, 34, 113, 20, 50,
		129, 145, 161, 8, 35, 66, 177, 193, 21,
		82, 209, 240, 36, 51, 98, 114, 130, 9,
		10, 22, 23, 24, 25, 26, 37, 38, 39, 40,
		41, 42, 52, 53, 54, 55, 56, 57, 58, 67,
		68, 69, 70, 71, 72, 73, 74, 83, 84, 85,
		86, 87, 88, 89, 90, 99, 100, 101, 102,
		103, 104, 105, 106, 115, 116, 117, 118,
		119, 120, 121, 122, 131, 132, 133, 134,
		135, 136, 137, 138, 146, 147, 148, 149,
		150, 151, 152, 153, 154, 162, 163, 164,
		165, 166, 167, 168, 169, 170, 178, 179,
		180, 181, 182, 183, 184, 185, 186, 194,
		195, 196, 197, 198, 199, 200, 201, 202,
		210, 211, 212, 213, 214, 215, 216, 217,
		218, 225, 226, 227, 228, 229, 230, 231,
		232, 233, 234, 241, 242, 243, 244, 245,
		246, 247, 248, 249, 250, 1, 0, 3, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 17,
		0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0,
		1, 2, 119, 0, 1, 2, 3, 17, 4, 5, 33, 49,
		6, 18, 65, 81, 7, 97, 113, 19, 34, 50,
		129, 8, 20, 66, 145, 161, 177, 193, 9,
		35, 51, 82, 240, 21, 98, 114, 209, 10,
		22, 36, 52, 225, 37, 241, 23, 24, 25, 26,
		38, 39, 40, 41, 42, 53, 54, 55, 56, 57,
		58, 67, 68, 69, 70, 71, 72, 73, 74, 83,
		84, 85, 86, 87, 88, 89, 90, 99, 100, 101,
		102, 103, 104, 105, 106, 115, 116, 117,
		118, 119, 120, 121, 122, 130, 131, 132,
		133, 134, 135, 136, 137, 138, 146, 147,
		148, 149, 150, 151, 152, 153, 154, 162,
		163, 164, 165, 166, 167, 168, 169, 170,
		178, 179, 180, 181, 182, 183, 184, 185,
		186, 194, 195, 196, 197, 198, 199, 200,
		201, 202, 210, 211, 212, 213, 214, 215,
		216, 217, 218, 226, 227, 228, 229, 230,
		231, 232, 233, 234, 242, 243, 244, 245,
		246, 247, 248, 249, 250, 255};
	memcpy(m_pBuf, HuffTb, 421);
	m_Index = 0; // Huffman Table을 읽어들이기 위하여 Index를 Reset
	FindDHT();
	_jpgWrite(hFile, (LPSTR)m_pBuf, 420);
	delete [] m_pBuf;
}
void CJpeg::PutSOF(HFILE hFile, int Width, int Height)
{
	WORD Marker, SegSize, w;
	BYTE c;
	int i;
	Marker = (0xc0 << 8) | 0xff;
	_jpgWrite(hFile, (LPSTR)&Marker, 2);
	SegSize = 17; SegSize = (SegSize << 8) | (SegSize >> 8);
	_jpgWrite(hFile, (LPSTR)&SegSize, 2); // Segment Size
	c = 8; _jpgWrite(hFile, (LPSTR)&c, 1); // P = 8
	w = (WORD)Height; w = (w << 8) | (w >> 8);
	_jpgWrite(hFile, (LPSTR)&w, 2); // Number of Y Line
	w = (WORD)Width; w = (w << 8) | (w >> 8);
	_jpgWrite(hFile, (LPSTR)&w, 2); // Number of Y Line
	c = 3; _jpgWrite(hFile, (LPSTR)&c, 1); // Nf = 3
	for(i=1; i<=3; i++)
	{
		c = (BYTE)i; _jpgWrite(hFile, (LPSTR)&c, 1); // Component Identifier
		c = 17; _jpgWrite(hFile, (LPSTR)&c, 1);    // HiVi
		if(i==1)
			c=0;
		else
			c=1;
		_jpgWrite(hFile, (LPSTR)&c, 1);    // Tqi
	}
}
void CJpeg::PutSOS(HFILE hFile)
{
	WORD Marker, SegSize;
	BYTE c;
	int i;
	Marker = (0xda << 8) | 0xff;
	_jpgWrite(hFile, (LPSTR)&Marker, 2);
	SegSize = 12; SegSize = (SegSize << 8) | (SegSize >> 8);
	_jpgWrite(hFile, (LPSTR)&SegSize, 2); // Segment Size
	c = 3; _jpgWrite(hFile, (LPSTR)&c, 1); // Ns = 3
	for(i=1; i<=3; i++)
	{
		c = (BYTE)i; _jpgWrite(hFile, (LPSTR)&c, 1); // Scan Component Selector
		if(i==1)
			c=0;
		else
			c=0x11;
		_jpgWrite(hFile, (LPSTR)&c, 1); // TdTa
	}
	c = 0; _jpgWrite(hFile, (LPSTR)&c, 1); // Ss
	c = 0x3f; _jpgWrite(hFile, (LPSTR)&c, 1); // Se
	c = 0; _jpgWrite(hFile, (LPSTR)&c, 1); // AhAl
}
void CJpeg::DCT(short *pos, int bWidth, BOOL Flag)
{
	// DCT 분만 아니라, DCT 후의 Zigzag 까지... 게다가 Quantization 까지!?//
	BYTE Qtb0[64] ={16, 11, 12, 14, 12, 10, 16, 14,
		13, 14, 18, 17, 16, 19, 24, 40,
		26, 24, 22, 22, 24, 49, 36, 37,
		29, 40, 58, 51, 61, 60, 57, 51,
		56, 55, 64, 72, 92, 78, 64, 68,
		87, 69, 66, 57, 80, 109, 81, 87,
		95, 98, 103, 104, 103, 62, 77, 113,
		121, 112, 100, 120, 92, 101, 103, 99};
	BYTE Qtb1[64] ={17, 18, 18, 24, 21, 24, 47, 26,
		26, 47, 99, 66, 56, 66, 99, 99,
		99 ,99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99 };
	int i, j;
	int x, y, u, v;
	float Cu, Cv;
	float Sum;
	float dct_coeff[8][8] =
	{
		+1.0f,  +1.0f,    +1.0f,  +1.0f,    +1.0f,  +1.0f,    +1.0f,  +1.0f,
		+0.9808f, +0.8315f, +0.5556f, +0.1951f, -0.1951f, -0.5556f, -0.8315f, -0.9808f,
		+0.9239f, +0.3827f, -0.3827f, -0.9239f, -0.9239f, -0.3827f, +0.3827f, +0.9239f,
		+0.8315f, -0.1951f, -0.9808f, -0.5556f, +0.5556f, +0.9808f, +0.1951f, -0.8315f,
		+0.7071f, -0.7071f, -0.7071f, +0.7071f, +0.7071f, -0.7071f, -0.7071f, +0.7071f,
		+0.5556f, -0.9808f, +0.1951f, +0.8315f, -0.8315f, -0.1951f, +0.9808f, -0.5556f,
		+0.3827f, -0.9239f, +0.9239f, -0.3827f, -0.3827f, +0.9239f, -0.9239f, +0.3827f,
		+0.1951f, -0.5556f, +0.8315f, -0.9808f, +0.9808f, -0.8315f, +0.5556f, -0.1951f
	};
	for(v=0; v<8; v++)
	{
		for(u=0; u<8; u++)
		{
			Sum = 0;
			for(y=0; y<8; y++)
				for(x=0; x<8; x++)
					Sum = Sum + pos[(int)(y*bWidth+x)] * dct_coeff[u][x] * dct_coeff[v][y];
			Cu = 1.; if(u == 0) Cu = 0.7071f;
			Cv = 1.; if(v == 0) Cv = 0.7071f;
			ZZ[(int)(v*8+u)] = (short)(Cu * Cv * Sum / 4.);
		}
	}
	Zigzag2();
	if(Flag) // TRUE : Chrominance, FALSE : Luminance
		for(i=0; i<64; i++)
			ZZ[i] = ZZ[i] / Qtb1[i];
	else
		for(i=0; i<64; i++)
			ZZ[i] = ZZ[i] / Qtb0[i];
	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
			pos[i*bWidth+j] = ZZ[i*8+j];
}
void CJpeg::Zigzag2()
{
	int Index[64] =
	{0,  1,  5,  6,  14, 15, 27, 28,
	2,  4,  7,  13, 16, 26, 29, 42,
	3,  8,  12, 17, 25, 30, 41, 43,
	9,  11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54,
	20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61,
	35, 36, 48, 49, 57, 58, 62, 63};
	short Temp[64];
	memcpy(Temp, ZZ, 64 * sizeof(short));
	int i, j, idx;
	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
		{
			idx = (i<<3)+j;
			ZZ[Index[idx]] = Temp[idx];
		}
}
void CJpeg::hEncode(int bWidth, int bHeight)
{
	int i, j;
	for(i=0; i<bHeight; i = i + 8)
	{
		for(j=0; j<bWidth; j = j + 8)
		{
			EncodeDU(&Y[i*bWidth+j], FALSE, bWidth);
			EncodeDU(&Cb[i*bWidth+j], TRUE, bWidth);
			EncodeDU(&Cr[i*bWidth+j], TRUE, bWidth);
		}
	}
}
void CJpeg::EncodeDU(short *pos, BOOL Flag, int bWidth)
{
	WORD Code;
	BYTE Val, Size;
	BYTE SSSS, Rs;
	short XX[64], DIFF;
	BYTE ThDC, ThAC;
	int idx = 0;
	int i, j;
	ThDC = 0; ThAC = 16;
	if(Flag)
	{
		ThDC = 1; ThAC = 17;
	}
	for(i=0; i<8; i++)
	{
		for(j=0; j<8; j++)
		{
			XX[idx] = pos[i*bWidth+j];
			idx++;
		}
	}
	// DC Encode //
	DIFF = XX[0];
	Val = GetCategory(DIFF);
	Code = TbH[ThDC].HUFFCODE[Val];
	Size = TbH[ThDC].HUFFSIZE[Val];
	if(DIFF < 0)
		DIFF--;
	ChargeCode(Code, Size);
	ChargeCode((WORD)DIFF, Val);
	// AC Encode //
	BYTE k = 0, R = 0;
	while(TRUE)
	{
		k++;
		if(XX[k] == 0)
		{
			if(k==63)
			{
				Rs = 0x00;
				Code = TbH[ThAC].HUFFCODE[TbH[ThAC].PT[Rs]];
				Size = TbH[ThAC].HUFFSIZE[TbH[ThAC].PT[Rs]];
				ChargeCode((WORD)Code, Size);
				break;
			}
			else
				R++;
		}
		else
		{
			while(R>15)
			{
				Rs = 0xf0;
				Code = TbH[ThAC].HUFFCODE[TbH[ThAC].PT[Rs]];
				Size = TbH[ThAC].HUFFSIZE[TbH[ThAC].PT[Rs]];
				ChargeCode((WORD)Code, Size);
				R = R - 16;
			}
			SSSS = GetCategory(XX[k]);
			//Rs = (R << 4) | SSSS;
			Rs = (R * 16) + SSSS;
			Code = TbH[ThAC].HUFFCODE[TbH[ThAC].PT[Rs]];
			Size = TbH[ThAC].HUFFSIZE[TbH[ThAC].PT[Rs]];
			ChargeCode((WORD)Code, Size);
			DIFF = XX[k];
			if(DIFF < 0)
				DIFF = DIFF - 1;
			ChargeCode((WORD)DIFF, SSSS);
			R = 0;
			if(k==63)
				break;
		}
	}
}
void CJpeg::ShotBit(BYTE Bit)
{
	static BYTE Bullet = 0;
	Bit = Bit << (7-cnt);
	Bullet = Bullet | Bit;
	cnt++;
	if(cnt == 8)
	{
		cnt = 0;
		m_pBuf[m_Index] = Bullet;
		m_Index++;
		if(Bullet == 0xff) // 0xff가 나오면 0x00으로 Byte Stuffing!!
		{
			m_pBuf[m_Index] = 0x00;
			m_Index++;
		}
		Bullet = 0;
	}
}
void CJpeg::PutEOI(HFILE hFile)
{
	// EOI 저장 //
	WORD Marker = (0xd9 << 8) | 0xff;
	_jpgWrite(hFile, (LPSTR)&Marker, 2);
}
void CJpeg::FindDHT()
{
	if( (m_pBuf[m_Index] == 0xff) && (m_pBuf[m_Index+1] == 0xc4) )
	{
		WORD SegSize = m_pBuf[m_Index+2]*256 + m_pBuf[m_Index+3];
		//버퍼의 현재 위치를 포인터로 설정한다.
		BYTE *p = &m_pBuf[m_Index+4];
		do
		{
			int i, j, k, LASTK;
			int Num = 0;
			BYTE BITS[17];
			BYTE Th = *p; // Table Number
			memcpy(BITS, p, 17);
			p = p + 17;

			//17개의 값을 모두 더해서 Num에 저장
			for(i=1; i<17; i++)
				Num = Num + BITS[i];
			TbH[Th].Flag = TRUE;
			TbH[Th].HUFFCODE = new WORD[Num+1];
			TbH[Th].HUFFSIZE = new BYTE[Num+1];
			TbH[Th].HUFFVAL = new BYTE[Num+1];

			//Huffman Value 값을 Num크기만큼 p에서 읽는다.
			memcpy(TbH[Th].HUFFVAL, p, Num);
			//p가 증가
			p = p + Num;
			// Generation of table of Huffman code sizes //
			i=1; j=1; k=0;
			while(i<=16)
			{
				while(j<=BITS[i])
				{
					TbH[Th].HUFFSIZE[k] = (BYTE)i;
					k++;
					j++;
				}
				i++;
				j=1;
			}
			TbH[Th].HUFFSIZE[k] = 0;
			LASTK = k;
			// Generation of table of Huffman codes
			k=0;
			WORD CODE = 0;
			BYTE SI = TbH[Th].HUFFSIZE[0];
			while(TRUE)
			{
				do
				{
					TbH[Th].HUFFCODE[k] = CODE;
					CODE++;
					k++;
				}while(TbH[Th].HUFFSIZE[k] == SI);
				if(TbH[Th].HUFFSIZE[k] == 0)
					break;
				do
				{
					CODE = CODE << 1;
					SI++;
				}while(TbH[Th].HUFFSIZE[k] != SI);
			}
			// Decoder table generation //
			i=0; j=0;
			while(TRUE)
			{
				do
				{
					i++;
					if(i>16)
						break;
					if(BITS[i] == 0)
						TbH[Th].MAXCODE[i] = -1;
				}while(BITS[i] == 0);
				if(i>16)
					break;
				TbH[Th].VALPTR[i] = j;
				TbH[Th].MINCODE[i] = TbH[Th].HUFFCODE[j];
				j = j + BITS[i] - 1;
				TbH[Th].MAXCODE[i] = TbH[Th].HUFFCODE[j];
				j++;
			}
			TbH[Th].Num = Num;
		}while(*p != 0xff);
		m_Index = m_Index + SegSize + 2;
	}
}
// 주어진 값이 어느 케티고리에 소속되는 지를 반환하는 함수 //
BYTE CJpeg::GetCategory(short V)
{
	BYTE Num = 0;
	if(V < 0)
		V = -V;
	while(V != 0)
	{
		V = V >> 1;
		Num++;
	}
	return Num;
}
void CJpeg::ChargeCode(WORD Code, int Size)
{
	int i;
	BYTE Bit;
	for(i=0; i<Size; i++)
	{
		Bit = (Code >> (Size - 1 - i)) & 0x01;
		ShotBit(Bit);
	}
}
