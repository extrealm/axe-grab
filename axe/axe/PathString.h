#pragma once

class CPathString
{
public:
	CPathString(void);
	~CPathString(void);

	CString CPathString::ConvertCygwinPath(LPCTSTR szPath);
};
