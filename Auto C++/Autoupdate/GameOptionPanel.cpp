// GameOptionPanel.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "GameOptionPanel.h"
//#include "../UPDATEDLL/src/UpdateExport.h"
//#include "../engine/Src/KWin32.h"
//#include "../engine/Src/KIniFile.h"
//#include "../engine/Src/KFilePath.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// GameOptionPanel dialog
const string FullScreenSectorName = "FullScreen";
const string RepresentSectorName  = "Represent";
const string DynaLightSectorName  = "DynamicLight";

const string FullScreenMode    = FullScreenSectorName + "=1";
const string WindowScreenMode  = FullScreenSectorName + "=0";

const string Represent2DMode   = RepresentSectorName + "=2";
const string Represent3DMode   = RepresentSectorName + "=3";

const string DynaLightMode     = DynaLightSectorName + "=1";
const string NotDynaLightMode  = DynaLightSectorName + "=0";

const string ConfileFileName   =  "config.ini";

const string Represent3DModuleFileName ="Represent3.dll";
const string Check_Represent_FunctionName = "";



const string MessageTitle = "JxoinlineOption";



GameOptionPanel::GameOptionPanel(CWnd* pParent /*=NULL*/)
	: CDialog(GameOptionPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(GameOptionPanel)
	m_2DOptionValue = -1;
	m_FullScreenValue = -1;
	m_DynaLightEnableValue = FALSE;
	m_txtCapPath = _T("");
	//}}AFX_DATA_INIT
}


void GameOptionPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GameOptionPanel)
	DDX_Control(pDX, IDC_WindowOption, m_WindowOptionCtl);
	DDX_Control(pDX, IDC_3DOption, m_3DOptionCtl);
	DDX_Control(pDX, IDC_FullScreen, m_FullScreenCtl);
	DDX_Control(pDX, IDC_2DOption, m_2DOptionCtl);
	DDX_Control(pDX, IDC_DynaLightEnable, m_DynaLightEnableCtl);
	DDX_Radio(pDX, IDC_2DOption, m_2DOptionValue);
	DDX_Radio(pDX, IDC_FullScreen, m_FullScreenValue);
	DDX_Check(pDX, IDC_DynaLightEnable, m_DynaLightEnableValue);
	DDX_Text(pDX, IDC_TXT_CAPPATH, m_txtCapPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GameOptionPanel, CDialog)
	//{{AFX_MSG_MAP(GameOptionPanel)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_3DOption, On3DOptionSelected)
	ON_BN_CLICKED(IDC_2DOption, On2DOption)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_BTN_CAPPATH, OnBtnCappath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GameOptionPanel message handlers

void GameOptionPanel::OnOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	try
	{
		//д��config.ini�ļ�
		char aConfigPath[MAX_PATH] ={0};
		theApp.g_GetRootPath(aConfigPath);
		strcat_s(aConfigPath, "\\");
		strcat_s(aConfigPath, ConfileFileName.c_str());
		char aCapPath[MAX_PATH] = {0};
		if (m_txtCapPath == "")
			theApp.g_GetRootPath(aCapPath);
		else
			strcpy_s(aCapPath, m_txtCapPath.GetBuffer(1));
		WritePrivateProfileString("Client","CapPath",aCapPath, aConfigPath);

		
		if (m_2DOptionCtl.GetCheck())
		{
			WritePrivateProfileString("Client","Represent","2", aConfigPath);
			WritePrivateProfileString("Client","DynamicLight","0", aConfigPath); 
		}
		else
		{
			WritePrivateProfileString("Client","Represent","3", aConfigPath);
			if (m_DynaLightEnableCtl.GetCheck())
				WritePrivateProfileString("Client","DynamicLight","1", aConfigPath);
			else
				WritePrivateProfileString("Client","DynamicLight","0", aConfigPath);
		}
		if (m_FullScreenCtl.GetCheck())
			WritePrivateProfileString("Client","FullScreen","1", aConfigPath);
		else
			WritePrivateProfileString("Client","FullScreen","0", aConfigPath);

		CDialog::OnOK();
	
	}
	catch(exception Error)
	{
		MessageBox(Error.what(),MessageTitle.c_str(),MB_ICONERROR);

	}
	

}

void GameOptionPanel::OnCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void GameOptionPanel::On3DOptionSelected() 
{
	// TODO: Add your control notification handler code here
	m_DynaLightEnableCtl.EnableWindow();

}

void GameOptionPanel::On2DOption() 
{
	// TODO: Add your control notification handler code here
	m_DynaLightEnableCtl.EnableWindow(FALSE);
	m_DynaLightEnableCtl.SetCheck(0);
	

}

BOOL GameOptionPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if(m_hIcon != NULL)
	{
		SetIcon(m_hIcon,FALSE);
	
		SetIcon(m_hIcon, TRUE);

	}
	else
	{
		DisplayErrorInfo(string("LoadIcon(IDR_MAINFRAME) ����") );
	}

	//��ȡconfig.ini�ļ�
	char aConfigPath[MAX_PATH] ={0};
	theApp.g_GetRootPath(aConfigPath);
	strcat_s(aConfigPath, "\\");
	strcat_s(aConfigPath, ConfileFileName.c_str());
	char aCapPath[MAX_PATH] = {0};
	GetPrivateProfileString("Client","CapPath","",aCapPath, MAX_PATH, aConfigPath);
	if (aCapPath[0] == '\0')
		theApp.g_GetRootPath(aCapPath);
	m_txtCapPath = aCapPath;
	UpdateData(false);

	const int nRepresentValue = GetPrivateProfileInt("Client","Represent", 3, aConfigPath);
	if( nRepresentValue == 2 )
	{
		m_2DOptionCtl.SetCheck(1);
		m_3DOptionCtl.SetCheck(0);
		m_DynaLightEnableCtl.SetCheck(0);
		m_DynaLightEnableCtl.EnableWindow(false);
	}
	else
	{
		m_2DOptionCtl.SetCheck(0);
		m_3DOptionCtl.SetCheck(1);
		m_DynaLightEnableCtl.EnableWindow();
		const int nDynamicLight = GetPrivateProfileInt("Client","DynamicLight", 1, aConfigPath);
		if (nDynamicLight == 0)
			m_DynaLightEnableCtl.SetCheck(0);
		else
			m_DynaLightEnableCtl.SetCheck(1);
	}
	const int nFullScreenValue = GetPrivateProfileInt("Client","FullScreen", 1, aConfigPath);
	if (nFullScreenValue == 0)
	{
		m_FullScreenCtl.SetCheck(0);
		m_WindowOptionCtl.SetCheck(1);
	}
	else
	{
		m_FullScreenCtl.SetCheck(1);
		m_WindowOptionCtl.SetCheck(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void GameOptionPanel::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	DeleteObject(m_hIcon);
}

void GameOptionPanel::OnDefault() 
{

}

void GameOptionPanel::OnBtnCappath() 
{
	UpdateData();
	LPITEMIDLIST pidlRoot=NULL; 
	SHGetSpecialFolderLocation(m_hWnd,CSIDL_DESKTOP,&pidlRoot); 

	BROWSEINFO bi;   //���봫��Ĳ���,�����������ṹ�Ĳ����ĳ�ʼ�� 
	//CString strDisplayName;   //�����õ�,��ѡ��Ļ�ҳ��·��,�൱���ṩһ�������� 
	bi.hwndOwner=GetSafeHwnd();   //�õ�������Handleֵ 
	bi.pidlRoot=pidlRoot;   //���������������������õ���. 
	bi.pszDisplayName=NULL;//strDisplayName.GetBuffer(MAX_PATH+1);   //�õ�������ָ��, 
	bi.lpszTitle="����ļ���";   //���ñ��� 
	bi.ulFlags=0;   //���ñ�־ 
	bi.lpfn=NULL; 
	bi.lParam=0; 
	bi.iImage=0;   //���������һЩ�޹صĲ���������,�����������, 
	LPITEMIDLIST pIIL =SHBrowseForFolder(&bi);   //�򿪶Ի��� 
	
	if(pIIL == NULL)return;
	//strDisplayName.ReleaseBuffer();   //�������GetBuffer()���Ӧ 

	TCHAR szInitialDir[MAX_PATH];
	BOOL bRet = ::SHGetPathFromIDList(pIIL, (char*)&szInitialDir);

	m_txtCapPath = szInitialDir;
	UpdateData(false);
}
