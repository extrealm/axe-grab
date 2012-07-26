#include "StdAfx.h"
#include "PathString.h"

CPathString::CPathString(void)
{
}

CPathString::~CPathString(void)
{
}

CString CPathString::ConvertCygwinPath(LPCTSTR szPath)
{
	CString str(szPath);
	str.Replace(_T('\\'), _T('/'));
	str.Replace(_T(':'), _T('/'));
	str = CString(_T("/cygdrive/")) + str;

	return str;
}