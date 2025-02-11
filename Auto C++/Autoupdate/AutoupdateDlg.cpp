// ReSharper disable CppMemberFunctionMayBeStatic
#include "stdafx.h"
#include "Autoupdate.h"
#include "AutoupdateDlg.h"
#include "SiteDlg.h"
//#include "StartDlg.h"

#define HOST_UPDATE "http://103.153.79.74/volam1_test/";
#define GAME_NAME	"game.exe";

char url[MAX_PATH];
int height = 270;

CAutoupdateDlg *dlg = NULL;
bool bStop = false;

#include "../UPDATEDLL/src/UpdateExport.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BITMAP bmp;
BOOL bRetry = FALSE;
bool bRunning = false;
bool bReboot = false;
bool bEnterGame = false;

BOOL OnExecute(LPCSTR szFile)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	::GetStartupInfo(&si);
    ZeroMemory( &pi, sizeof(pi) );
	
	char buffer[256];
	
	strcpy_s(buffer, szFile);
	
	BOOL bRet = TRUE;
	
    if( !CreateProcess( NULL, buffer, NULL,	NULL, FALSE, 0,	NULL, NULL,	&si, &pi)) {
		bRet = FALSE;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	
	return bRet;
}

void DisplayErrorInfo(string& ErrorInfo) {
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	MessageBox( NULL, ErrorInfo.c_str(),(LPCTSTR)lpMsgBuf , MB_OK |MB_ICONWARNING );
	LocalFree( lpMsgBuf );
}


int __stdcall RefreshStatus(int nCurrentStatus, long lParam)
{
	if(bStop) return bStop;
	LPDOWNLOADFILESTATUS pFileinfo = NULL;
	switch(nCurrentStatus)
	{
	case defUPDATE_STATUS_INITIALIZING:
		dlg->m_DisplayVersionCtl.SetWindowText("K’t nËi m∏y chÒ...");
		dlg->m_progress.SetPos(20);
		break;
	case defUPDATE_STATUS_VERIFING:
		dlg->m_DisplayVersionCtl.SetWindowText("Ki”m tra phi™n b∂n...");
		dlg->m_progress.SetPos(40);
		break;
	case defUPDATE_RESULT_VERSION_NOT_ENOUGH:
		{
			strcpy_s(url, (char *)lParam);
			AnnuncePanel NeedDownAnnuncePanel;
			strcpy_s(NeedDownAnnuncePanel.url, url);
			NeedDownAnnuncePanel.m_ResModule = dlg->m_ResModule;
			NeedDownAnnuncePanel.DoModal();
			bStop = true;
		}
//–Ë“™“ª¥Œ–‘ ÷∂Ø…˝º∂∞¸
		break;
	case defUPDATE_STATUS_PROCESSING_INDEX:
		dlg->m_DisplayVersionCtl.SetWindowText("ßang k’t nËi m∏y chÒ...");
		dlg->m_progress.SetPos(60);
		break;
	case defUPDATE_STATUS_DOWNLOADING:
		dlg->m_DisplayVersionCtl.SetWindowText("Æang t∂i d˜ li÷u...");
		dlg->m_progress.SetPos(80);
		break;
	case defUPDATE_STATUS_DOWNLOADING_FILE:
		dlg->m_progress.SetPos(80);
		pFileinfo = (LPDOWNLOADFILESTATUS)lParam;
		if (pFileinfo) {
			CString strFile;
			strFile.Format("%s --- %d KB(%d KB)", pFileinfo->strFileName, pFileinfo->dwFileSize / 1024, pFileinfo->dwFileDownloadedSize / 1024);
			dlg->m_DisplayVersionCtl.SetWindowText(strFile);
			dlg->m_progressCurrent.SetPos((int)(pFileinfo->dwFileDownloadedSize * 100.0 / pFileinfo->dwFileSize));
		}
		break;
	case defUPDATE_STATUS_UPDATING:
		dlg->m_DisplayVersionCtl.SetWindowText("ßang cÀp nhÀt h÷ thËng...");
		dlg->m_progress.SetPos(100);
		break;
	case defUPDATE_RESULT_UPDATE_SUCCESS:
		dlg->m_DisplayVersionCtl.SetWindowText("CÀp nhÀt thµnh c´ng...");
		dlg->m_progress.SetPos(100);
		dlg->m_progressCurrent.SetPos(100);
		dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
		dlg->m_CancelButton.ShowWindow(SW_HIDE);
		dlg->m_EnterGameButton.MoveWindow(bmp.bmWidth - 120, height - 60, 90, 24);

		dlg->m_EnterGameButton.EnableWindow(true);
		break;
	case defUPDATE_RESULT_VERSION_MORE:
		dlg->m_DisplayVersionCtl.SetWindowText("Phi™n b∂n Æang mÌi nh t kh´ng c«n cÀp nhÀt ");
		dlg->m_progress.SetPos(100);
		dlg->m_progressCurrent.SetPos(100);
		dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
		dlg->m_CancelButton.ShowWindow(SW_HIDE);
		dlg->m_EnterGameButton.MoveWindow(bmp.bmWidth - 120, height - 60, 90, 24);

		dlg->m_EnterGameButton.EnableWindow(true);
		break;
	default:
		ASSERT(FALSE);
	}
	return bStop;
}

BOOL g_bLog = FALSE;

typedef int (__stdcall *UPDATEA_INIT)(KUPDATE_SETTING UpdateSetting);
typedef int (__stdcall *UPDATE_UNINIT)();
typedef int (__stdcall *UPDATE_START)();
typedef int (__stdcall *UPDATE_CANCEL)();

UPDATEA_INIT g_Update_Init = NULL;
UPDATE_UNINIT g_Update_UnInit = NULL;
UPDATE_START g_Update_Start = NULL;
UPDATE_START g_Update_Cancel = NULL;

HMODULE hModule;
UINT AutoUpdateDLL(char* pSite)
{
    KUPDATE_SETTING update_set;
    int n_ret_code = 0;
    CString s_text;
	bRunning = true;

    update_set.bAutoTryNextHost = false;
    update_set.bUseVerify = false;
    update_set.bUseFastestHost = false;
    update_set.pfnCallBackProc = &RefreshStatus;
    update_set.ProxySetting.bUpdateAuth = true; 
    update_set.ProxySetting.nHostPort = 0;
    update_set.ProxySetting.nProxyMethod = PROXY_METHOD_DIRECT;
    strcpy_s(update_set.szVerifyInfo, "102400-010999-106075-054738");
    update_set.ProxySetting.szHostAddr[0] = '\0';
    update_set.ProxySetting.szPassword[0] = '\0';
    update_set.ProxySetting.szUserName[0] = '\0';
	
    
	char sz_module_path[MAX_PATH + 1];
	::GetModuleFileName(NULL, sz_module_path, (MAX_PATH + 1));
    char* pszOffset = strrchr(sz_module_path, '\\');
    ASSERT(pszOffset);
	strcpy_s(update_set.szMainExecute, pszOffset + 1);
    pszOffset[1] = '\0';
	strcpy_s(update_set.szDownloadPath, sz_module_path);
	strcat_s(update_set.szDownloadPath, "Update\\");
	strcpy_s(update_set.szUpdatePath, sz_module_path);
	
	update_set.nUpdateMode = 0;

	
	strncpy_s(update_set.szUpdateSite, pSite, MAX_PATH);

    const CString strVersion = sz_module_path;
	update_set.nVersion = ::GetPrivateProfileInt(
		"Version",
		"Version",
		0,
		strVersion + "Version.cfg");

	
    update_set.bLog = g_bLog;
	if (g_Update_Init == NULL || g_Update_UnInit == NULL || g_Update_Start == NULL) {
		goto Exit0;
	}

    n_ret_code = g_Update_Init(update_set);
	Sleep(10);
    if (n_ret_code == defUPDATE_RESULT_INIT_FAILED)
        goto Exit0;
	
    n_ret_code = g_Update_Start();
    g_Update_UnInit();

Exit0:
	update_set.nVersion = ::GetPrivateProfileInt("Version", "Version", 0, strVersion + "Version.cfg");
	bRunning = false;
    return n_ret_code;
}

UINT UpdateThread(LPVOID p)
{
	
	bStop = false;
	Sleep(10);
	dlg->m_DisplayProgressCtl.SetWindowText("Hi÷n tπi");
	dlg->m_DisplayVersionCtl.SetWindowText("ßang n©ng c p t˘ ÆÈng");
	dlg->m_DisplayCurrentCtl.SetWindowText("Toµn bÈ");

	const int ret = AutoUpdateDLL((char*)p);

	if (ret >= 0 && (ret == defUPDATE_RESULT_INIT_FAILED || ret == defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED ||
		ret == defUPDATE_RESULT_PROCESS_INDEX_FAILED || ret == defUPDATE_RESULT_CONNECT_SERVER_FAILED ||
		ret == defUPDATE_RESULT_DOWNLOAD_FAILED)) {
		dlg->m_nCurrentHost++;
		if(dlg->m_nCurrentHost >= dlg->m_strHosts.GetSize()) {
			//dlg->m_DisplayVersionCtl.SetWindowText("Œﬁø…”√µƒ∏¸–¬∑˛ŒÒ∆˜£¨ƒ˙ø…“‘≥¢ ‘Ω¯»Î”Œœ∑£¨µ´ «ø…ƒ‹ª·≥ˆœ÷¥ÌŒÛ");
			dlg->m_DisplayVersionCtl.SetWindowText("K’t nËi m∏y chÒ th t bπi");
			dlg->m_progress.SetPos(00);
			dlg->m_progressCurrent.SetPos(0);
			//dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
			//dlg->m_EnterGameButton.EnableWindow();
			//dlg->m_EnterGameButton.MoveWindow(bmp.bmWidth - 106, height - 60, 90, 24);
			
			dlg->m_RetryButton.ShowWindow(SW_SHOW);

			dlg->m_EnterGameButton.EnableWindow(false);
		}
		else {
			dlg->UpdateFromCurrentHost();
		}
	}
	else if (ret == defUPDATE_RESULT_CANCEL) {
		dlg->EndDialog(IDCANCEL);
//		dlg->m_web.Stop();
//		dlg->EndDialog(IDCANCEL);
	}
	else if(ret == 100) {
		dlg->EndDialog(IDCANCEL);
	}
	else if(ret == 101) {
//		dlg->EndDialog(IDCANCEL);
	}
	else if (ret == defUPDATE_RESULT_UPDATESELF_SUCCESS) {
		FILE *site_ini = fopen(dlg->m_strSiteList, "w+");
		if(site_ini) {
			char line[100];
			sprintf_s(line, "%d\n", dlg->m_nCurrentHost);
			fputs(line, site_ini);
			for(int index = 0; index < dlg->m_strHosts.GetSize(); index++) {
				sprintf_s(line, "%s\n", dlg->m_strHosts[index]);
				fputs(line, site_ini);
			}
			fclose(site_ini);
		}
//		g_Update_Cancel();
//		dlg->m_web.Stop();
		if (!dlg->m_strUpdateSelf.IsEmpty()) {
			dlg->m_strRun.Empty();
			bReboot = true;
		}
		dlg->EndDialog(IDCANCEL);
	}
	else {
		dlg->m_DisplayVersionCtl.SetWindowText("CÀp nhÀt thµnh c´ng...");
		dlg->m_progress.SetPos(100);
		dlg->m_progressCurrent.SetPos(100);
		dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
		dlg->m_CancelButton.ShowWindow(SW_HIDE);
		dlg->m_EnterGameButton.MoveWindow(bmp.bmWidth - 120, height - 60, 90, 24);

		dlg->m_EnterGameButton.EnableWindow(true);
	}
	::SetWindowText(dlg->m_hWnd, "AutoUpdate");

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateDlg dialog
//const CString ResFileName = "AutoUpdateRes.dll";
CAutoupdateDlg::CAutoupdateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoupdateDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CAutoupdateDlg)
	m_strVersion = _T("");
	m_strProgress = _T("");
	m_strProgressCurrent = _T("");
	m_nCurrentHost = 0;//**ƒ⁄≤‡»˝
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

CAutoupdateDlg::~CAutoupdateDlg()
{
	if(m_ResModule)
	{
		if(!FreeLibrary(m_ResModule))
		{                           
//			DisplayErrorInfo(string("FreeLibrary()"));
		}
        m_ResModule = NULL;
	}
//‘⁄¥À‘À––≥Ã–Ú
	if(bReboot) {
		OnExecute(m_strUpdateSelf);
	}
	else if(bEnterGame) {
		OnExecute(m_strRun);
	}
}

void CAutoupdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoupdateDlg)
	DDX_Control(pDX, IDC_RETRY, m_RetryButton);
	DDX_Control(pDX, IDC_REGUSER, m_RegUserCtl);
	DDX_Control(pDX, IDC_SINA, m_SinaCtl);
	DDX_Control(pDX, IDC_17173, m_17173Ctl);
	DDX_Control(pDX, IDC_HOME, m_HomeCtl);
	DDX_Control(pDX, IDC_JOYO, m_JoyoCtl);
	DDX_Control(pDX, IDC_CARD, m_CardCtl);
	DDX_Control(pDX, IDC_CONFIG, m_GameOption);
	DDX_Control(pDX, IDC_PROGRESSCURRENT, m_progressCurrent);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_STATIC_PROGRESS, m_DisplayProgressCtl);
	DDX_Control(pDX, IDC_STATIC_PROGRESSCURRENT, m_DisplayCurrentCtl);
	DDX_Control(pDX, IDC_STATIC_VERSON, m_DisplayVersionCtl);
	DDX_Control(pDX, IDC_REGISTER, m_RegisterCtl);
	DDX_Control(pDX, IDC_INVEST, m_InvestCtl);
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
	DDX_Control(pDX, IDOK, m_EnterGameButton);
	DDX_Text(pDX, IDC_STATIC_VERSON, m_strVersion);
	DDX_Text(pDX, IDC_STATIC_PROGRESS, m_strProgress);
	DDX_Text(pDX, IDC_STATIC_PROGRESSCURRENT, m_strProgressCurrent);
	DDX_Control(pDX, IDC_EXPLORER1, m_web);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutoupdateDlg, CDialog)
//{{AFX_MSG_MAP(CAutoupdateDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_REGISTER, OnRegister)
	ON_BN_CLICKED(IDC_INVEST, OnInvest)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RETRY, OnRetry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateDlg message handlers
BOOL CAutoupdateDlg::OnInitDialog()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CDialog::OnInitDialog();

	if (!m_strWebURL.IsEmpty()) {

		m_web.Navigate(m_strWebURL, NULL, NULL, NULL, NULL);
	}
	m_progress.SetRange(0, 100);
	m_progressCurrent.SetRange(0, 100);

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	const HBITMAP temp_bit_map = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_InfoTitle));
	if (!m_bmTitle.Attach(temp_bit_map)) {
	}
	if (!((HBITMAP)m_bmTitle)) {
	    PostQuitMessage(1);
	}
	m_bmTitle.GetBitmap(&bmp);	
	
    CRect WindowRange(0,0,bmp.bmWidth, height);
	MoveWindow(WindowRange);
	CenterWindow(NULL);

	::SetWindowText(m_hWnd, "AutoUpdate");

	m_EnterGameButton.ShowWindow(SW_HIDE);

	m_RegisterCtl.ShowWindow(SW_HIDE);
	m_InvestCtl.ShowWindow(SW_HIDE);

	m_CancelButton.MoveWindow(bmp.bmWidth - 120, height - 70, 90, 24);
	m_RetryButton.MoveWindow(bmp.bmWidth - 220, height - 70, 90, 24);
	m_EnterGameButton.EnableWindow(false);

	m_GameOption.MoveWindow(bmp.bmWidth - 120, bmp.bmHeight + 50, 90, 24);

	m_progress.MoveWindow(115, bmp.bmHeight + 35, bmp.bmWidth - 256, 15);
    m_progressCurrent.MoveWindow(115, bmp.bmHeight + 60, bmp.bmWidth - 256, 15);

	m_DisplayVersionCtl.MoveWindow(10, bmp.bmHeight + 10, bmp.bmWidth - 240,15);
	m_DisplayProgressCtl.MoveWindow(10, bmp.bmHeight + 35, 100,15);
	m_DisplayCurrentCtl.MoveWindow(10, bmp.bmHeight + 60, 100,15);
	
	m_CardCtl.ShowWindow(SW_HIDE);
	m_CardCtl.MoveWindow(14, height - 65, 70, 24);
	m_CardCtl.SetHyperlink("http://202.108.250.168/SwordOnline/showFillAction.do?action=fillvalue");

	m_RegUserCtl.ShowWindow(SW_HIDE);
	m_RegUserCtl.MoveWindow(90, height - 65, 70, 24);
	m_RegUserCtl.SetHyperlink("http://202.108.250.168/SwordOnline/account.do?action=Create");

	m_HomeCtl.ShowWindow(SW_HIDE);
	m_HomeCtl.MoveWindow(166, height - 65, 70, 24);
	m_HomeCtl.SetHyperlink("http://www.jxonline.net");

	m_JoyoCtl.ShowWindow(SW_HIDE);
	m_JoyoCtl.MoveWindow(19, height - 45, 70, 24);
	m_JoyoCtl.SetHyperlink("http://www.joyo.net/");

	m_SinaCtl.ShowWindow(SW_HIDE);
	m_SinaCtl.SetFontColor(RGB(255,0,0));
	m_SinaCtl.MoveWindow(90, height - 45, 70, 24);
	m_SinaCtl.SetHyperlink("http://games.sina.com.cn/z/jx/");

	m_17173Ctl.ShowWindow(SW_HIDE);
	m_17173Ctl.MoveWindow(166, height - 45, 70, 24);
	m_17173Ctl.SetHyperlink("http://jx.17173.com");


	m_web.MoveWindow(0, bmp.bmHeight + 85, bmp.bmWidth - 8, height - bmp.bmHeight - 160);
	m_web.ShowWindow(SW_HIDE);
	m_web.EnableWindow(false);
//	UpdateFinish.SetEvent();
	UpdateFromCurrentHost();
	
//	SetTimer(200, 50, NULL);
	dlg = this;
	//m_nCurrentHost = -1;

	m_bEnableRun = FALSE;
    
	hModule = LoadLibrary("UpdateDLL.dll");
    if (!hModule)
        return FALSE;
	
    g_Update_Init = (UPDATEA_INIT)GetProcAddress(hModule, "Update_Init");
    if (!g_Update_Init)
        return FALSE;
	
    g_Update_UnInit = GetProcAddress(hModule, "Update_UnInit");
    if (!g_Update_UnInit)
        return FALSE;
	
    g_Update_Start = GetProcAddress(hModule, "Update_Start");
    if (!g_Update_Start)
        return FALSE; 
	
	g_Update_Cancel = GetProcAddress(hModule, "Update_Cancel");
    if (!g_Update_Cancel)
        return FALSE;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CAutoupdateDlg::InitAutoUpdate()
{
	g_bLog = 0;
	char sz_module_path[MAX_PATH + 1];
	theApp.g_GetRootPath(sz_module_path);
	const CString str_path = sz_module_path;
	m_strWebURL = "https://www.google.com.vn/?hl=vi";
	const CString str_site = HOST_UPDATE
	m_strRun = str_path + "\\" +GAME_NAME
	m_strUpdateSelf = str_path + "\\Update\\" + "UpdateSelf.DAT";
	m_strSiteList = str_path + "\\Update\\" + "SiteList.ini";
	m_nCurrentHost = -1;
	m_strHosts.RemoveAll();
	//


    std::string command = "start /b reg add \"HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\" /v \"%s\" /t REG_SZ /d \"~ 16BITCOLOR\" /f";
    size_t pos = command.find("%s");
    if (pos != std::string::npos) {
        command.replace(pos, 2, m_strRun);
        std::system(command.c_str());
    }
    std::string command1 = "start /b reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\" /v \"%s\" /t REG_SZ /d \"DisableNXShowUI\" /f";
    size_t pos1 = command1.find("%s");
    if (pos1 != std::string::npos) {
        command1.replace(pos1, 2, m_strRun);
        std::system(command1.c_str());
    }
	//
	FILE *site_ini = fopen(m_strSiteList, "r");
	if(site_ini) {
		char line[100];
		while(fgets(line, 100, site_ini) != NULL) {
			if(m_nCurrentHost == -1) {
				m_nCurrentHost = atoi(line);
				if(m_nCurrentHost == -1) m_nCurrentHost = 0;
			}
			else {
				int last = strlen(line) - 1;
				if(line[last] == '\n') {
					line[last--] = 0;
				}
				if(line[last] == '\r') {
					line[last] = 0;
				}
				m_strHosts.Add(line);
			}
		}
		fclose(site_ini);
		remove(m_strSiteList);
		if(m_nCurrentHost >= m_strHosts.GetSize()) m_nCurrentHost = 0;
		return (m_strHosts.GetSize() >= 0);
	}

	char sz_site[MAX_PATH];
	int n = 1;

	
	CStringArray strHosts;
		sz_site[0] = 0;
		strcpy_s(sz_site, str_site);
		if (sz_site[0] != 0)
			strHosts.Add(sz_site);
	if (strHosts.GetSize() > 0)
	{
		CSiteDlg sites;
		sites.site_list = &strHosts;
		int n_start = rand() * GetTickCount() % strHosts.GetSize();

		sites.nStart = n_start;
		/*
		if(sites.DoModal() != IDOK) {
			return FALSE;
		}*/
		n_start = sites.nStart;

		int i;
		for (i = n_start; i < strHosts.GetSize(); i++)
		{
			m_strHosts.Add(strHosts[i]);
		}
		for (i = 0; i < n_start; i++)
		{
			m_strHosts.Add(strHosts[i]);
		}
		ASSERT(m_strHosts.GetSize() == strHosts.GetSize());
		m_nCurrentHost = 0;

		m_nConnectionStep = ConnectionProgress/(m_strHosts.GetSize());
	}
	else
		m_nCurrentHost = -1;

	return (m_nCurrentHost >= 0);
}

void CAutoupdateDlg::UpdateFromCurrentHost() {
	if (m_nCurrentHost >= 0 && m_nCurrentHost < m_strHosts.GetSize() )
	{
		AfxBeginThread(UpdateThread, (void*)(LPCTSTR)m_strHosts[m_nCurrentHost]);
	}
}

void CAutoupdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CAutoupdateDlg::OnPaint() 
{
	CPaintDC PaintDC(this); 
	if (IsIconic())
	{
		// Center icon in client rectangle
		const int cxIcon = GetSystemMetrics(SM_CXICON);
		const int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		PaintDC.DrawIcon(x, y, m_hIcon);
	}
	else {
		//ª≠±ÍÃ‚
        CDC dc;
        CBitmap *pOldBitmap = NULL;
		dc.CreateCompatibleDC(&PaintDC);
		
        
        pOldBitmap = dc.SelectObject(&m_bmTitle);

		PaintDC.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dc, 0, 0, SRCCOPY);

        dc.SelectObject(pOldBitmap);

       
	}
}

HCURSOR CAutoupdateDlg::OnQueryDragIcon() {
	return (HCURSOR) m_hIcon;
}

void CAutoupdateDlg::OnCancel() {
	m_web.Stop();
	const CWnd* p = GetDlgItem(IDCANCEL);
	CString str;
	p->GetWindowText(str);
	m_DisplayVersionCtl.SetWindowText("HÒy cÀp nhÀt...");
	m_progress.SetPos(0);
	m_progressCurrent.SetPos(0);
	bStop = true;
	if (g_Update_Cancel) {
		g_Update_Cancel();
	}
	EndDialog(IDCANCEL);
}

void CAutoupdateDlg::OnDestroy()
{
//	WaitForSingleObject(UpdateFinish.m_hObject, INFINITE);
	while(bRunning) {
		Sleep(100);
	}
	if (hModule) {
      FreeLibrary(hModule);
	  hModule = NULL;
	}

	CDialog::OnDestroy();
}

void CAutoupdateDlg::OnOK()
{
	if (!m_strRun.IsEmpty())
	{
		bEnterGame = true;
	}
	
	EndDialog(IDOK);
}
 
BEGIN_EVENTSINK_MAP(CAutoupdateDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CAutoupdateDlg)
	ON_EVENT(CAutoupdateDlg, IDC_EXPLORER1, 271 /* NavigateError */, OnNavigateErrorExplorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CAutoupdateDlg::OnRegister() {
//	ShellExecute(NULL,"open","\\\\s\\◊‘∂Ø∏¸–¬\\Ω£œ¿«È‘µ3Õ¯…œΩª“◊∆ΩÃ®.htm",NULL,NULL,SW_SHOW);
}

void CAutoupdateDlg::OnInvest() 
{
//	ShellExecute(NULL,"open","\\\\s\\◊‘∂Ø∏¸–¬\\Ω£œ¿«È‘µ3Õ¯…œΩª“◊∆ΩÃ®.htm",NULL,NULL,SW_SHOW);
}

void CAutoupdateDlg::LoadBitMapFromInstance(CBitmap &Dest, HINSTANCE Hinstance, WORD wResourceID) {
    HBITMAP TempBitMap = LoadBitmap(Hinstance,MAKEINTRESOURCE(wResourceID));
	if(!TempBitMap)	{
		DisplayErrorInfo(string("LoadBitmap()"));
	}
		
	if(!Dest.Attach(TempBitMap)) {
		DisplayErrorInfo(string("FromHandle()"));
	}
}

HBRUSH CAutoupdateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	const HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(nCtlColor == CTLCOLOR_STATIC   )
	{
		pDC->SetTextColor(RGB(102,101,98));
		// Set the background mode for text to transparent 
		// so background will show thru.
		pDC->SetBkMode(TRANSPARENT);
	}
	return hbr;
}

void CAutoupdateDlg::OnConfig() 
{
	GameOptionPanel OptionInterface;
	OptionInterface.DoModal();
}

const char *HOME_PAGE = "http://www.jxonline.net";
void CAutoupdateDlg::OnNavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Frame, VARIANT FAR* StatusCode, BOOL FAR* Cancel) 
{
	if(!bRetry) {
		bRetry = TRUE;
		m_web.Navigate(HOME_PAGE, NULL, NULL, NULL, NULL);
	}
}

void CAutoupdateDlg::OnRetry() 
{
	//÷ÿ ‘¡¨Ω”
	m_RetryButton.ShowWindow(SW_HIDE);
	m_CancelButton.ShowWindow(SW_SHOW);
	m_nCurrentHost = 0;
	UpdateFromCurrentHost();

	dlg = this;
	//m_nCurrentHost = -1;

	m_bEnableRun = FALSE;

}
