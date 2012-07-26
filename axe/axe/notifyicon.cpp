#include "stdafx.h"
// #include <windows.h>
#include <shellapi.h>

#include "notifyicon.h"

static BOOL AddModifyNotifyIcon(DWORD dwMessage,HWND hwnd,UINT uID,
                                HICON hicon,LPCTSTR lpszTip,UINT uMessage)
{
	NOTIFYICONDATA notifyicondata;

	notifyicondata.cbSize=sizeof(NOTIFYICONDATA);
	notifyicondata.hWnd=hwnd;
	notifyicondata.uID=uID;

	notifyicondata.uFlags=0;

	if(dwMessage==NIM_ADD)
	{
		notifyicondata.uFlags|=NIF_MESSAGE;
		notifyicondata.uCallbackMessage=uMessage;
	}

	if(hicon!=NULL)
	{
		notifyicondata.uFlags|=NIF_ICON;
		notifyicondata.hIcon=hicon;
	}

	if(lpszTip!=NULL)
	{
		notifyicondata.uFlags|=NIF_TIP;
		lstrcpyn(notifyicondata.szTip,lpszTip,sizeof(notifyicondata.szTip));
	}

	return !Shell_NotifyIcon(dwMessage,&notifyicondata);
}

BOOL AddNotifyIcon(HWND hwnd,UINT uID,HICON hicon,LPCTSTR lpszTip,UINT uMessage)
{
	return AddModifyNotifyIcon(NIM_ADD,hwnd,uID,hicon,lpszTip,uMessage);
}

BOOL ModifyNotifyBarIcon(HWND hwnd,UINT uID,HICON hicon,LPCTSTR lpszTip)
{
	return AddModifyNotifyIcon(NIM_MODIFY,hwnd,uID,hicon,lpszTip,0);
}

BOOL DeleteTaskBarIcon(HWND hwnd,UINT uID)
{
	NOTIFYICONDATA tnid;

	tnid.cbSize=sizeof(NOTIFYICONDATA);
	tnid.hWnd=hwnd;
	tnid.uID=uID;

	return !Shell_NotifyIcon(NIM_DELETE,&tnid);
}

