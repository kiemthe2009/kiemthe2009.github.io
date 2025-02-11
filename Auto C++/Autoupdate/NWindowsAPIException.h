#pragma once
#include "nbaseexception.h"

class NWindowsAPIException :
	public NBaseException
{
public:
	//���캯����ͨ��GetLastError()ȡ��ϵͳģ����󣬲���ʼ��ErrorType,ErrorInfo
	NWindowsAPIException(const char * ThrowFileName,int ThrowLineNumber,const char * ModuleName);
	virtual ~NWindowsAPIException(void);
};
void AssertWindowsAPI(int result,char * FileName,int Line,char * ModuleName);

#define AssertSystemAPI(result) AssertWindowsAPI(result,__FILE__,__LINE__,NULL)
#define AssertModuleAPI(result,ModuleName) AssertWindowsAPI(result,__FILE__,__LINE__,ModuleName)
