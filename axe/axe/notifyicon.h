#ifndef _NOTIFYICON_H_
#define _NOTIFYICON_H_

#include <windows.h>
#include <shellapi.h>

#define UM_TRAYICON	(WM_USER+10)

BOOL AddNotifyIcon(HWND hwnd,UINT uID,HICON hicon,LPCTSTR lpszTip,UINT uMessage);

BOOL ModifyNotifyBarIcon(HWND hwnd,UINT uID,HICON hicon,LPCTSTR lpszTip);

BOOL DeleteTaskBarIcon(HWND hwnd,UINT uID);

#endif