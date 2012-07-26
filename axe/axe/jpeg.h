
///////////////////
// Jpeg.h
///////////////////
class CJpeg
{
public:
	CJpeg(void);
	virtual ~CJpeg(void);
	typedef struct _DHT {
		int    Num;          // 허프만 코드의 수
		int    VALPTR[17];   // 해당 길이의 코드가 시작되는 인덱스
		int    *PT;          // VALUE로 INDEX를 빠르게 찾기 위한 포인터
		int        cnt; // 비트단위로 연산할 때 쓰이는 카운터
		BOOL   Flag;         // 사용되었는지 여부를 나타내는 플래그
		BYTE   *HUFFSIZE;    // 허프만 코드의 길이
		BYTE   *HUFFVAL;     // 허프만 코드가 나타내는 값
		WORD   *HUFFCODE;    // 허프만 코드
		WORD   MAXCODE[17];  // 해당 길이에서 가장 큰 코드
		WORD   MINCODE[17];  // 해당 길이에서 가장 작은 코드
	} DHT;
	int m_imageSize;
	int m_Index;                // 버퍼의 위치를 나타내는 인덱스
	int m_rWidth;               // 이미지의 실제적인 넓이
	int m_rHeight;              // 이미지의 실제적인 높이
	int cnt;                    // 비트단위로 연산할 때 쓰이는 카운터
	DHT TbH[20];                // Huffman Table
	short ZZ[64];               // 8x8 Block 정보를 담는 배열
	short *Y;                   // Save할 때 쓰이는 Y Buffer
	short *Cb;                  // Save할 때 쓰이는 Cb Buffer
	short *Cr;                  // Save할 때 쓰이는 Cr Buffer
	BYTE*           m_pBuf;     // 버퍼
	BYTE*           m_pData;    // 이미지 버퍼
	LPSTR           m_pDib;      // 실제 픽셀데이터를 저장하는 구조체
	BITMAPINFO      m_pBitmapInfo;   //저장하기 위해서 비트맵의 정보가 들어갈 구조체
	void ChargeCode(WORD Code, int Size);
	void FindDHT();
	void PutEOI(HFILE hFile);
	void ShotBit(BYTE Bit);
	void EncodeDU(short *pos, BOOL Flag, int bWidth);
	void hEncode(int bWidth, int bHeight);
	void Zigzag2();
	void DCT(short *pos, int bWidth, BOOL Flag);
	void PutSOS(HFILE hFile);
	void PutSOF(HFILE hFile, int Width, int Height);
	void PutDHT(HFILE hFile);
	void PutDQT(HFILE hFile);
	void PutSOI(HFILE hFile);
	void FlipY();
	SIZE GetSize();
	BYTE GetCategory(short V);
	BOOL LoadMemDIB(HANDLE hDIB);
	BOOL SaveJPG(LPCTSTR file);
	BOOL SaveJPG(LPCTSTR FileName, int Width, int Height, BYTE *pp);  // JPEG 파일을 저장하는 함수
};
