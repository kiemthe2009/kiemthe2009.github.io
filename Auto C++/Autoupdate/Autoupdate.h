// Autoupdate.h : main header file for the AUTOUPDATE application
//

#if !defined(AFX_AUTOUPDATE_H__D4E4CC3B_07EF_4279_A014_4B15DA3760FA__INCLUDED_)
#define AFX_AUTOUPDATE_H__D4E4CC3B_07EF_4279_A014_4B15DA3760FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#ifndef MAXPATH
#define MAXPATH   260
#define MAXDIR    256
#define MAXFILE   256
#define MAXEXT    256
#define MAXDRIVE    3
#endif
class CAutoupdateApp : public CWinApp
{
public:
	CAutoupdateApp();
	~CAutoupdateApp();
	BOOL IsTheSelfRun();
	BOOL IsTheGameRun();
	HANDLE m_hMutex;
	CString m_strMutex;

	
	public:
	virtual	int		g_StrLen(LPCSTR lpStr);
	virtual	void	g_GetRootPath(LPSTR lpPathName);
	virtual	void	g_SetRootPath(LPSTR lpPathName = NULL);
	virtual	void	g_StrCpy(LPSTR lpDest, LPCSTR lpSrc);
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CAutoupdateApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPDATE_H__D4E4CC3B_07EF_4279_A014_4B15DA3760FA__INCLUDED_)
