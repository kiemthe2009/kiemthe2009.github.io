//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdatePublic.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-1-17 11:15:44
//  Comment     :   Inplement the comment interface of update system
//
//////////////////////////////////////////////////////////////////////////////////////
                        
#include "Stdafx.h"    
#include "Global.h"
#include "UpdatePublic.h"
#include "SaveLog.h"
#include "UpdateSelf.h"

#include "KAVStrTranslate.h"
#include "GetVersion.h"
#include "GetFilesVersion.h"

#include "PublicFun.h"
#include "ProcessIndex.h"
#include "UpdateData.h"
#include "Resource.h"
#include "SourceDef.h"

CProcessIndex g_ProcessIndex;
extern KUPDATE_DATA g_UpdateData;

///////////////////////////////////////////////////////////////////////////////
//更新包合并 Add By Fellow 2003.08.11
#define MAX_FILE					2004800			//最多20万个文件

char temp_buffer[MAX_FILE] = {0};

int getIndex(t_index_info* IndexList,int Count, unsigned long id)
{//二分法找到指定的索引
	int nBegin, nEnd, nMid;
	nBegin = 0;
	nEnd = Count;
	while (nBegin <= nEnd) {
		nMid = (nBegin + nEnd) / 2;
		if (id == IndexList[nMid].id) break;
		if (id < IndexList[nMid].id) nEnd = nMid - 1;
		else nBegin = nMid + 1;
	}
	if(id != IndexList[nMid].id) return -1;								//数据文件里面也没有
	return nMid;
	
}
void GetHeader(t_pack_header* aHeader, FILE* aFile)
{
	fseek(aFile,0,SEEK_SET);
	fread(aHeader,1,sizeof(t_pack_header),aFile);
}

void GetIndexInfo(t_index_info* aIndexInfo, const t_pack_header* aHeader, FILE* aFile)
{
	fseek(aFile,aHeader->index_offset,SEEK_SET);
	fread(aIndexInfo, sizeof(t_index_info), aHeader->count, aFile);
}

bool PackComb(const char* stdFilePath,const char* addFilePath)
{//合并文件
	t_pack_header stdHeader;
	t_pack_header addHeader;
	unsigned long offset;
	t_index_info* stdIndex;
	t_index_info* addIndex;
	//打开资源源文件
	FILE* stdFile =NULL;
	stdFile = fopen(stdFilePath, "r+b");
	if(stdFile == NULL)
	{
		return false;
	}
	GetHeader(&stdHeader, stdFile);

	//打开资源新增加文件
	FILE* addFile =NULL;
	addFile = fopen(addFilePath, "rb");
	if(addFile == NULL)
	{
		return false;
	}
	GetHeader(&addHeader, addFile);

	stdIndex =  new t_index_info[stdHeader.count + addHeader.count];
	memset(stdIndex,0,sizeof(t_index_info) * (stdHeader.count + addHeader.count));
	GetIndexInfo(stdIndex, &stdHeader, stdFile);

	addIndex =  new t_index_info[addHeader.count];
	memset(addIndex,0,sizeof(t_index_info) * addHeader.count);
	//如果addFile里面没有需要增加的数据就直接完成
	if (addHeader.count == 0)
	{//关闭文件
		fclose(stdFile);
		fclose(addFile);
		return true;
	}
	GetIndexInfo(addIndex, &addHeader, addFile);
	
	//copy增加的数据到源文件中
	int DataSize = addHeader.index_offset - addHeader.data_offset;
	char* DataBuffer = new char[DataSize];
	fseek(addFile,addHeader.data_offset,SEEK_SET);
	fread(DataBuffer, 1, DataSize, addFile);

	fseek(stdFile,0,SEEK_END);
	offset = ftell(stdFile);
	char *ptr = DataBuffer;
	while(DataSize > 0) {
		int result;
		if(DataSize > 0x100000) result = fwrite(ptr ,1,0x100000,stdFile);
		else result = fwrite(ptr ,1, DataSize,stdFile);
		DataSize -= result;
		ptr += result;
	}
	//重新建立index
	int i,j;
	for(i = 0; i < addHeader.count; i++)
	{
		t_index_info* index = &addIndex[i];
		for(j = 0; j < stdHeader.count; j++) {
			if(index->id < stdIndex[j].id)
			{	//索引排序
				memcpy(temp_buffer, (char *)&stdIndex[j], (stdHeader.count - j) * sizeof(t_index_info));
				memcpy((char *)&stdIndex[j + 1], temp_buffer, (stdHeader.count - j) * sizeof(t_index_info));
				//添加新索引
				memcpy(&stdIndex[j], index, sizeof(t_index_info));
				stdIndex[j].offset = offset + index->offset - sizeof(t_pack_header);
				++stdHeader.count;
				break;
			}
			else if(index->id == stdIndex[j].id)
			{	//新索引替换旧索引
				stdIndex[j].offset = offset + index->offset - sizeof(t_pack_header);
				stdIndex[j].size = index->size;
				stdIndex[j].compress_size = index->compress_size;
				break;
			}
		}
		if(j == stdHeader.count) {
			memcpy(&stdIndex[j], index, sizeof(t_index_info));
			stdIndex[j].offset = offset + index->offset - sizeof(t_pack_header);//Add By Fellow 09.04
			stdHeader.count++;
		}
	}

	//写入新的index
	fseek(stdFile,0,SEEK_END);
	offset = ftell(stdFile);
	fwrite(stdIndex,sizeof(t_index_info),stdHeader.count,stdFile);
	//写入header
	stdHeader.index_offset = offset;
	fseek(stdFile,0,SEEK_SET);
	fwrite(&stdHeader,sizeof(t_pack_header),1,stdFile);

	//关闭文件
	delete [] DataBuffer;
	delete [] stdIndex;
	delete [] addIndex;
	fclose(stdFile);
	fclose(addFile);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------

int ProcessIndexFile(const char cszFileName[])
{
    int nResult                     = false;
    int nRetCode                    = 0;
    char szIndexTxtFile[MAX_PATH]   = {0};
    CString sLog                    = "";

    if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
        strcpy(szIndexTxtFile, g_PathTable.szTempPath);
    else
        strcpy(szIndexTxtFile, g_PathTable.szDownloadDestPath);
    strcat(szIndexTxtFile, defINDEX_TXT_FILE_NAME);

    nRetCode = g_ProcessIndex.Init(cszFileName, szIndexTxtFile);
    if (!nRetCode)
    {
        if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
            sLog = defIDS_ERROR_INDEX;
        else 
            sLog = defIDS_INDEX_ERROR;

        g_UpdateData.SaveLog.WriteLogString(sLog, true);

        goto Exit0;
    }
    
    nRetCode = g_ProcessIndex.AnalysisUpdateFiles(        
        g_UpdateData.nMethod,                  
        g_UpdateData.szLocalPath, 
        g_UpdateData.nMainVersion
    );
    if (!nRetCode)
    {
         if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
            sLog = defIDS_ERROR_INDEX;
        else 
            sLog = defIDS_INDEX_ERROR;

        g_UpdateData.SaveLog.WriteLogString(sLog, true);

        goto Exit0;
    }
  
	if(nRetCode ==defVersionNotenough || nRetCode ==defVersionMore)
	{
		return nRetCode;
	}
	
    g_UpdateData.bNeedUpdateSelfFirst   = g_ProcessIndex.IsNeedUpdateSelf();
    g_UpdateData.bVersionError      = g_ProcessIndex.IsVersionError();

    // Count file size of Program or engie and sign or tools and help         

    nResult = true;

Exit0:    
    return nResult;
}

//////////////////////////////////////////////////////////////////////////////////
// Update file to local
//////////////////////////////////////////////////////////////////////////////////
int inline _GetTempFileName(char szFileName[])
{
    int i               = 0;
    int nFileNameLen    = 0;
    int nStrSize        = 0;
    char *pszStr        = NULL;
    int nRetCode        = 0;
    
    nFileNameLen    = strlen(szFileName);
    pszStr          = szFileName + nFileNameLen;
    nStrSize        = MAX_PATH - nFileNameLen;   

    while (i < INT_MAX)
    {
        _IntToStr(i, nStrSize, pszStr);
        nRetCode = FileExists(szFileName);
        if (!nRetCode)
            break;
        i++;
    }
    
    return true;
}

static int inline _GetOSPlatformVersion()
{
    OSVERSIONINFOEX     osVersionInfo;  
    int nRetCode = 0;
    int nResult  = -1;

    ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    nRetCode = ::GetVersionEx((OSVERSIONINFO *)&osVersionInfo);
    if (!nRetCode)
    {
        osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        nRetCode = ::GetVersionEx((OSVERSIONINFO *)&osVersionInfo);
        KAV_PROCESS_ERROR(nRetCode);
    }
  
    switch(osVersionInfo.dwPlatformId)
    {
    case VER_PLATFORM_WIN32_NT:
        nResult = defOSPLATFORM_WINNT;
        break;
    case VER_PLATFORM_WIN32_WINDOWS:
        if (((osVersionInfo.dwBuildNumber >> 16) & 0x0000FFFF) < 0x045A)
            nResult = defOSPLATFORM_WIN9598;
        else 
            nResult = defOSPLATFORM_WINME;
        break;
    default:
        nResult = 0;
        break;
    }
    
Exit0:
    return nResult;
}

int CloseProgram(const char cszFileName[])
{
    int nResult = false;
    int nRetCode = 0;
    KModuleExitInfo ModuleExitInfo;
    KBaseClosedProgram *pClosedProgram = NULL;
    KProcessInfo ProcessInfo;


    ModuleExitInfo.nExitType = -1;
    strcpy(ModuleExitInfo.szFileName, cszFileName);
    ModuleExitInfo.uID = 0;
    ModuleExitInfo.szStartParam[0] = '\0';
    ModuleExitInfo.szStopParam[0] = '\0';
	ModuleExitInfo.szReleaseFileName[0] = '\0';

    nRetCode = g_ProcessIndex.GetExitType(&ModuleExitInfo);    
    KAV_PROCESS_ERROR(nRetCode);
    
    ProcessInfo.uID = ModuleExitInfo.uID;
    strcpy(ProcessInfo.szFileName, ModuleExitInfo.szReleaseFileName);
    strcpy(ProcessInfo.szStartParam, ModuleExitInfo.szStartParam);
    strcpy(ProcessInfo.szCloseParam, ModuleExitInfo.szStopParam);


    switch (ModuleExitInfo.nExitType)
    {
    case defEXIT_TYPE_DIRECTORY:
        pClosedProgram = new KClosedProgram(ProcessInfo);
        KAV_PROCESS_ERROR(pClosedProgram);   

        break;

    default:
        goto Exit0;		
    }
    
    nRetCode = g_UpdateData.CloseProgramMgr.AddClosedProgram(pClosedProgram);
    if (!nRetCode)
    {
        delete pClosedProgram;
        pClosedProgram = NULL;
    }
    nResult = pClosedProgram->CloseProgram();
    
Exit0:
    return nResult;
}


BOOL AddRenameSectionContent(CString sPathName, CString sContent)
{
    CFile file;
    CString sOld, sNew, sPart1, sPart2;

    if (FileExists(sPathName))
    {
        if (file.Open(sPathName, CFile::modeRead | CFile::shareDenyNone))
        {
            int nLen = file.GetLength();
            char *pBuf = new char [nLen + 1];

            file.Read(pBuf, nLen);
            pBuf[nLen] = '\0';

            sOld = pBuf;
            KAV_DELETE_ARRAY(pBuf);

            file.Close();
        }
        else
            return FALSE;
    }
    else
    {
        sOld = "";
    }
    CString sTemp = sOld;
    CString sTemp2 = sContent;
    
    sTemp.MakeLower();
    sTemp2.MakeLower();
    
    if (sTemp.Find(sTemp2) != -1)
        return true;
    
    int nPos = sTemp.Find("[rename]\r\n");
    if (nPos != -1)
    {
        sPart1 = sOld.Left(nPos + 10);
        sPart2 = sOld.Mid(nPos + 10);
        if (sContent.Right(2) != "\r\n")
            sContent += "\r\n";

        sNew = sPart1 + sContent + sPart2;
    }
    else
    {
        sPart1 = sOld;
        sPart2 = "\r\n[rename]\r\n";
        
        sNew = sPart1 + sPart2 + sContent;
    }

    if (file.Open(sPathName, CFile::modeCreate | CFile::modeWrite))
    {
        file.Write(sNew, sNew.GetLength());

        file.Close();
    }
    else
        return FALSE;

    return TRUE;
}


int KAVSetupInstallFile(const char cszCleanedFileName[], const char cszLockedFileName[])
{
    int nResult = false;
    int nRetCode = false;
    char szTempFileName[MAX_PATH] = {0};
    char szTemp[MAX_PATH] = {0};
    CString szWININITFileName;
    char *pszPos = NULL;
    CString szBuf;
    char szCleanedFileShortName[MAX_PATH] = {0};
    char szLockedFileShortName[MAX_PATH] = {0};

    ::GetShortPathName(cszCleanedFileName, szCleanedFileShortName, MAX_PATH);
    ::GetShortPathName(cszLockedFileName, szLockedFileShortName, MAX_PATH);

    strcpy(szTempFileName, szLockedFileShortName);
    pszPos = strrchr(szTempFileName, '.');
    pszPos++;
    *pszPos = '\0';
    strcat(pszPos, "TMP");
    
    nRetCode = ::CopyFile(szCleanedFileShortName, szTempFileName, FALSE);
    KAV_PROCESS_ERROR(nRetCode);
    
    szBuf = "";
    szBuf += "NUL=" + (CString)szLockedFileShortName + "\r\n";
    szBuf += (CString)szLockedFileShortName + "=" + (CString)szTempFileName + "\r\n";


    ::GetShortPathName(g_PathTable.szWindowsPath, szTemp, MAX_PATH);
    szWININITFileName = szTemp;
    szWININITFileName += "Wininit.ini";

    ::AddRenameSectionContent(szWININITFileName, szBuf);
    
    nResult = true;

Exit0:
    return nResult;
}


// cszDestFileName : Dest file name, need update
// cszTempFileName : temp file
int ProcessSharingFile(const char cszDestFileName[], const char cszTempFileName[])
{
    int nResult                         = false;
    int nRetCode                        = 0;
    char szLocalTempFileName[MAX_PATH]  = {0};
    char szLockedFileShortName[MAX_PATH]= {0};
    HANDLE hFile                        = INVALID_HANDLE_VALUE;
    int nWritten                        = 0;
    char szBuf[MAX_PATH]                = {0};
    CString szCopyIniFileName;    
    CFile File;
    CString sAttrib_FileName;
    CString sKAVMOVE_BAT_FileName;
    CString sKAVMove_EXE_FileName;
    CString sCleanedFileShortName;
    CString sLockedFileShortName;
    CString sExeFilePathShort;
    CString sBuffer;
    int nCount = 0;

    if (g_UpdateData.nOSPlatVersion == -1)        // Get the operation system platform ID
        g_UpdateData.nOSPlatVersion = _GetOSPlatformVersion();

    switch (g_UpdateData.nOSPlatVersion)
    {
    case defOSPLATFORM_WINNT:                 // WINNT/Windows2000
        nRetCode = ::SetFileAttributes(cszDestFileName, FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL);
        KAV_PROCESS_ERROR(nRetCode);
        
        strcpy(szLocalTempFileName, cszDestFileName);
        _GetTempFileName(szLocalTempFileName); 
        nRetCode = ::CopyFile(cszTempFileName, szLocalTempFileName, false);
        KAV_PROCESS_ERROR(nRetCode);
            
        //::DeleteFile(cszTempFileName);

        nRetCode = ::MoveFileEx(
            szLocalTempFileName, cszDestFileName, 
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_DELAY_UNTIL_REBOOT
        );
        KAV_PROCESS_ERROR(nRetCode);

        break;

    case defOSPLATFORM_WIN9598:        // Windows 95/98/ME  
    case defOSPLATFORM_WINME:          // Windows ME
        break;

    }       // End of switch
    
    nResult = true;

Exit0:    
    return nResult;
}

int UpdateSelf()
{
    int nResult                         = false;
    int nRetCode                        = 0;
    char szUpdateSelfFile[MAX_PATH]     = {0};
    char szUpdateSelfInfoFile[MAX_PATH] = {0};
    char szParameter[MAX_PATH]          = {0};
    char szDstFile[MAX_PATH]            = {0};
    char szKeyName[30]                  = {0};
    CString sLog                        = "";
    PKUPDATE_ITEM pUpdateItem           = NULL;
    int nNum                            = 0;


    if (!g_UpdateData.bNeedUpdateSelfFirst)
        goto Exit0;

    // Get full path name for the Update Self information file
    strcpy(szUpdateSelfInfoFile, g_PathTable.szDownloadDestPath);  
	MkDirEx(szUpdateSelfInfoFile);
    strcat(szUpdateSelfInfoFile, defUPDATE_SELF_INFO_FILE);
	

    // Delete the old information file
    ::DeleteFile(szUpdateSelfInfoFile);
    
    nNum = 0;
    pUpdateItem = g_ProcessIndex.m_pUpdateItemList;
    while (pUpdateItem)
    {
        // Download temp file (srcfile)
        sprintf(szKeyName, "SrcFile%d", nNum);
        WritePrivateProfileString(
            "UpdateSelf",
	        szKeyName,
	        pUpdateItem->szDownloadTmpFileName,
	        szUpdateSelfInfoFile
        );
	        
        //  DestFile
        sprintf(szKeyName, "DesFile%d", nNum);
        strcpy(szDstFile, pUpdateItem->szLocalPath);
        strcat(szDstFile, pUpdateItem->szFileName);
        WritePrivateProfileString(
	        "UpdateSelf",
	        szKeyName,
	        szDstFile,
	        szUpdateSelfInfoFile
        );

        pUpdateItem = pUpdateItem->pNext;
        nNum++;
    }

    // Parameter
    strcpy(szParameter, "/CONTINUE");
    strcat(szParameter, " ");
    strcat(szParameter, g_UpdateData.szParameter);
    WritePrivateProfileString(
		"UpdateSelf",
		"Parameter",
		szParameter,
		szUpdateSelfInfoFile
    );

    strcpy(szDstFile, g_PathTable.szUpdateDestPath);
    strcat(szDstFile, g_UpdateData.szExecuteProgram);
    WritePrivateProfileString(
		"UpdateSelf",
		"FullPathName",
		szDstFile,
		szUpdateSelfInfoFile
    );

    // Create UpdateSelf.DAT file for Update self
    strcpy(szUpdateSelfFile, g_PathTable.szDownloadDestPath);
    strcat(szUpdateSelfFile, defUPDATE_SELF_FILE);
    nRetCode = CreateUpdateSelfFile(szUpdateSelfFile);        
    if (nRetCode)
	{
		sLog = defIDS_UPDATE_SELF_SUCCESS;
		nResult = true;
	}
    else
	{
		sLog = defIDS_UPDATE_SELF_FAILED;
	}

    g_UpdateData.SaveLog.WriteLogString(sLog, true);

Exit0:

    return nResult;
}

int UpdateFile_Copy(const KUPDATE_ITEM& UpdateItem)
{
    int nResult                         = false;
    int nRetCode                        = false;
    char szLocalFileName[MAX_PATH]      = {0};
    char szLocalTempFileName[MAX_PATH]  = {0};
    CString sLog;
    CString sFormat;
    DWORD dwFileAttribute               = 0;

    strcpy(szLocalFileName, UpdateItem.szLocalPath);
    strcat(szLocalFileName, UpdateItem.szFileName); 
    MkDirEx(UpdateItem.szLocalPath);
	TRACE1("%s is copied\n", szLocalFileName);

    dwFileAttribute = GetFileAttributes(szLocalFileName);
    dwFileAttribute &= ~dwFileAttribute;
    SetFileAttributes(szLocalFileName, dwFileAttribute);
	dwFileAttribute = GetFileAttributes(UpdateItem.szDownloadTmpFileName);
	dwFileAttribute &= ~dwFileAttribute;
	SetFileAttributes(UpdateItem.szDownloadTmpFileName, dwFileAttribute);

    nRetCode = ::CopyFile(UpdateItem.szDownloadTmpFileName, szLocalFileName, false);
    if (nRetCode)
    {
        nResult = CheckFileCRC(atoi(UpdateItem.szCRC), szLocalFileName);        
        if (!nResult)
        {
            g_UpdateData.bUpdateFailed = true;
            sFormat = defIDS_UPDATE_FAILED;
            goto Exit0;
        }
        if (UpdateItem.nReboot)
        {
            g_UpdateData.bNeedRebootFalg = true;

            sFormat = defIDS_NEED_RESET;
        }
        else
        {
            sFormat = defIDS_UPDATE_SUCCESS;
        }     

		::DeleteFile(UpdateItem.szDownloadTmpFileName);

        goto Exit0;
    }

    // Update after reset computer 
    strcpy(szLocalTempFileName, g_PathTable.szTempPath);
    strcat(szLocalTempFileName, UpdateItem.szFileName);
    _GetTempFileName(szLocalTempFileName);
    nRetCode = ::CopyFile(UpdateItem.szDownloadTmpFileName, szLocalTempFileName, false);
    if (!nRetCode)
    {
        sFormat = defIDS_COPY_TEMPFILE_FAILED;        
        g_UpdateData.bUpdateFailed = true;
        goto Exit0;
    }
    
    nRetCode = ProcessSharingFile(szLocalFileName, szLocalTempFileName);
    if (!nRetCode)
    {
        sFormat = defIDS_PROCESS_SHARING_FAILED;
        
        g_UpdateData.bUpdateFailed = true;
        goto Exit0;
    }
    
    nResult = CheckFileCRC(atoi(UpdateItem.szCRC), szLocalTempFileName);
    if (!nResult)
    {
        g_UpdateData.bUpdateFailed = true;
        sFormat = defIDS_UPDATE_FAILED;
        goto Exit0;
    }
    
    g_UpdateData.bRebootFinishUpdateFlag = true;

    if (UpdateItem.nSharedNeedReboot || UpdateItem.nReboot)
    {
        sFormat = defIDS_UPDATE_SUCCESS_NEED_RESET;
        g_UpdateData.bNeedRebootFalg = true;
    }
    else
    {
        sFormat = defIDS_UPDATE_SHARING_SUCCESS;
    }    
    

Exit0:

    sLog.Format(sFormat, UpdateItem.szFileName, UpdateItem.szLocalPath);
    g_UpdateData.SaveLog.WriteLogString(sLog, true);

    return nResult;
}

int UpdateFile(KUPDATE_ITEM& UpdateItem)
{
	UpdateFile_Copy(UpdateItem);
	return true;
}

int UpdateFiles()
{
    KUPDATE_ITEM *pUpdateItem   = NULL;
    int nRetCode                = 1;

    if (g_UpdateData.bNeedUpdateSelfFirst)
    {
        nRetCode = UpdateSelf();         
        return nRetCode;
    }
		
    pUpdateItem = g_ProcessIndex.m_pUpdateItemList;

	KUPDATE_ITEM *pLastUpdateItem  = NULL;

    while (pUpdateItem)
    {
        if (
            (pUpdateItem->DownloadStatus == enumDOWNLOADSTATUS_DOWNLOADED) &&
            pUpdateItem->bNeedUpdate
        )
		{
			if (strnicmp(pUpdateItem->szUpdateFileMethod, "Last;", 5) == 0 &&
				pLastUpdateItem == NULL) //the first last copy
			{
				pLastUpdateItem = pUpdateItem;
				memmove(pLastUpdateItem->szUpdateFileMethod, pLastUpdateItem->szUpdateFileMethod + 5, MAX_PATH - 5);
			}
            else if (!UpdateFile(*pUpdateItem))
				nRetCode = 0;
		}

        pUpdateItem = pUpdateItem->pNext;
    }

	if (pLastUpdateItem && !g_UpdateData.bUpdateFailed)
	{
		if (!UpdateFile(*pLastUpdateItem))
			nRetCode = 0;
	}

    if (g_UpdateData.bRebootFinishUpdateFlag)
    {
    }

    return nRetCode;
}

int GetHostURL(const char cszFileName[], const char cszHostName[], char szHostURL[])
{
	CStdioFile file;
	int nResult = false;


	if (file.Open(cszFileName, CFile::modeRead | CFile::typeText))
	{
		CString sName, sAddress;

		while (file.ReadString(sName))
		{
			sName.TrimLeft();
			sName.TrimRight();

			if (sName == "")
				continue;

            while (file.ReadString(sAddress))
			{
				sAddress.TrimLeft();
				sAddress.TrimRight();

				if (sAddress == "")
					continue;

				if (stricmp(cszHostName, sName) == 0)
				{
					strcpy(szHostURL, sAddress);
					nResult = true;
					break;
				}

                break;
			}
		}

		file.Close();
	} 

	return nResult;
}
