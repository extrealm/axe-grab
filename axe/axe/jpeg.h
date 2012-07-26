
///////////////////
// Jpeg.h
///////////////////
class CJpeg
{
public:
	CJpeg(void);
	virtual ~CJpeg(void);
	typedef struct _DHT {
		int    Num;          // ������ �ڵ��� ��
		int    VALPTR[17];   // �ش� ������ �ڵ尡 ���۵Ǵ� �ε���
		int    *PT;          // VALUE�� INDEX�� ������ ã�� ���� ������
		int        cnt; // ��Ʈ������ ������ �� ���̴� ī����
		BOOL   Flag;         // ���Ǿ����� ���θ� ��Ÿ���� �÷���
		BYTE   *HUFFSIZE;    // ������ �ڵ��� ����
		BYTE   *HUFFVAL;     // ������ �ڵ尡 ��Ÿ���� ��
		WORD   *HUFFCODE;    // ������ �ڵ�
		WORD   MAXCODE[17];  // �ش� ���̿��� ���� ū �ڵ�
		WORD   MINCODE[17];  // �ش� ���̿��� ���� ���� �ڵ�
	} DHT;
	int m_imageSize;
	int m_Index;                // ������ ��ġ�� ��Ÿ���� �ε���
	int m_rWidth;               // �̹����� �������� ����
	int m_rHeight;              // �̹����� �������� ����
	int cnt;                    // ��Ʈ������ ������ �� ���̴� ī����
	DHT TbH[20];                // Huffman Table
	short ZZ[64];               // 8x8 Block ������ ��� �迭
	short *Y;                   // Save�� �� ���̴� Y Buffer
	short *Cb;                  // Save�� �� ���̴� Cb Buffer
	short *Cr;                  // Save�� �� ���̴� Cr Buffer
	BYTE*           m_pBuf;     // ����
	BYTE*           m_pData;    // �̹��� ����
	LPSTR           m_pDib;      // ���� �ȼ������͸� �����ϴ� ����ü
	BITMAPINFO      m_pBitmapInfo;   //�����ϱ� ���ؼ� ��Ʈ���� ������ �� ����ü
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
	BOOL SaveJPG(LPCTSTR FileName, int Width, int Height, BYTE *pp);  // JPEG ������ �����ϴ� �Լ�
};
