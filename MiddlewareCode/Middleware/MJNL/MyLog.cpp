#include "StdAfx.h"
#include "MyLog.h"
//#define CONFIG_PATH "c:\\HMKiosk\\HMApp\\config.ini"
#include "XFSAPI.H"
#include "XFSIDC.H"
#include <tchar.h>
#include <stdio.h>

char    chSPName[20]={0};

void  SetSPName(char* chName)
{
	if (NULL !=chName)
	{
		strcpy(chSPName,chName);
	}
}

int WriteLog(char *fmt, ...)
{ 	
	char buffer[LOG_BUF_LEN] = { 0 };	
	char buf[LOG_BUF_LEN] = { 0 };
	
	char strTime[LOG_FILE_LEN] = { 0 };	
	
	SYSTEMTIME t;
	GetLocalTime(&t);	
	sprintf(strTime, "%04d-%02d-%02d %02d:%02d:%02d ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);	
	
	char f[MAX_PATH] = { 0 };
	char szPrefix[MAX_PATH] = { 0 };
	strcat(szPrefix,"c:\\HMMiddle\\log");
	sprintf(f, "%s\\%s_%04d%02d%02d.log", szPrefix,chSPName, t.wYear,t.wMonth,t.wDay);
	HMCreateDirectory(szPrefix);
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	memcpy(buf, buffer, sizeof(buffer));
	va_end(ap);
	
	int fd;    
	struct stat sbuf;
	stat(f, &sbuf);
	
	fd = open(f, O_CREAT | O_RDWR | O_TEXT, S_IREAD | S_IWRITE);
	
	if ( fd < 0 )
	{	
		return -2;
	}
	
	//通过月份判断删除上一个月的日志
	char date[16] = { 0 };
	read(fd, date, 10);
	char preMonth[3] = { 0 };
	preMonth[0] = date[5];
	preMonth[1] = date[6];
	if ( atoi(preMonth) != t.wMonth )
	{
		close(fd);
		fd = open(f, O_CREAT | O_RDWR | O_TRUNC | O_TEXT, S_IREAD | S_IWRITE);
		if ( fd < 0 )
		{	
			return -2;
		}
	}
	
	lseek(fd, 0, SEEK_END);    	
	
	write(fd, strTime, strlen(strTime));
	
	write(fd, buf, strlen(buf));
	
	write(fd, "\n", 1);
	
	close(fd);
	
	return 0;
}

int HMCreateDirectory(TCHAR* szPath)
{
	if ( ::PathFileExists(szPath) ) return 0;

	TCHAR DirName[MAX_PATH];
	TCHAR* p = szPath;
	TCHAR* q = DirName; 
	while ( *p )
	{
		if ( ('\\' == *p) || ('/' == *p) )
		{
			if ( ':' != *(p-1) )
			{
				CreateDirectory(DirName, NULL);
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	CreateDirectory(DirName, NULL);

	return 0;
} 

HMODULE GetCurrentModule(BOOL bRef)
{
	HMODULE hModule = NULL;
	if (GetModuleHandleEx(bRef ? GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS : 
		(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT), (LPCSTR)GetCurrentModule, &hModule))
	{
		return hModule;
	}
	return NULL;
}

HMODULE ModuleFromAddress()
{
	MEMORY_BASIC_INFORMATION mbi;

	return ((::VirtualQuery(ModuleFromAddress, &mbi, sizeof(mbi)) != 0) 
		? (HMODULE) mbi.AllocationBase : NULL);
}

void GetProgramVersion(__out char *pszVersion, __out char *pszName)
{
	if ( pszVersion == NULL || pszName == NULL ) return;

	TCHAR szFullPath[MAX_PATH] = { 0 };
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;
	HMODULE m;
// 	HMODULE m  = ModuleFromAddress();
// 	GetModuleFileName(m, szFullPath, sizeof(szFullPath));	
// 
// 	memset(szFullPath, 0, sizeof(szFullPath));
// 	m = GetCurrentModule(TRUE);	
// 	GetModuleFileName(m, szFullPath, sizeof(szFullPath));

	memset(szFullPath, 0, sizeof(szFullPath));
	m = GetCurrentModule(FALSE);	
	GetModuleFileName(m, szFullPath, sizeof(szFullPath));

	char szFName[MAX_PATH] = { 0 };
	char szEXT[MAX_PATH] = { 0 };	
	_splitpath(szFullPath, NULL, NULL, szFName, szEXT);
	sprintf(pszName, "%s%s", szFName, szEXT);

	dwVerInfoSize = ::GetFileVersionInfoSize(szFullPath, &dwVerHnd);
	if ( dwVerInfoSize )
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = GlobalLock(hMem);
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValue(lpvMem, (LPTSTR)("\\"), (void**)&pFileInfo, &uInfoSize);
		int ret = GetLastError();
		WORD wProductVersion[4] = { 0 };

		// Product version from the FILEVERSION of the version info resource 
		wProductVersion[0] = HIWORD(pFileInfo->dwProductVersionMS); 
		wProductVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
		wProductVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
		wProductVersion[3] = LOWORD(pFileInfo->dwProductVersionLS); 

		sprintf(pszVersion, ("%d.%d.%d.%d"), wProductVersion[0], 
			wProductVersion[1], wProductVersion[2], wProductVersion[3]);

		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}
} 

void HMPrintVersion()
{
	char szVersion[MAX_PATH]	= { 0 };
	char szFileName[MAX_PATH]	= { 0};
	GetProgramVersion(szVersion, szFileName);
	WriteLog("******************************************");
	WriteLog("");
	WriteLog("模块名:%s  版本号:%s", szFileName, szVersion);
	WriteLog("");
	WriteLog("******************************************");
}

#define HMXFSErrCode_Num ( (int) (sizeof HMXFSErrCode_Array / sizeof HMXFSErrCode_Array[0]) )
struct
{
	int     nValue;
	TCHAR   *szText;
}
HMXFSErrCode_Array[] =
{
	WFS_SUCCESS,                                   TEXT("WFS_SUCCESS"),
	WFS_ERR_ALREADY_STARTED,                       TEXT("WFS_ERR_ALREADY_STARTED"),
	WFS_ERR_API_VER_TOO_HIGH,                      TEXT("WFS_ERR_API_VER_TOO_HIGH"),
	WFS_ERR_API_VER_TOO_LOW,                       TEXT("WFS_ERR_API_VER_TOO_LOW"),
	WFS_ERR_CANCELED,                              TEXT("WFS_ERR_CANCELED"),
	WFS_ERR_CFG_INVALID_HKEY,                      TEXT("WFS_ERR_CFG_INVALID_HKEY"),
	WFS_ERR_CFG_INVALID_NAME,                      TEXT("WFS_ERR_CFG_INVALID_NAME"),
	WFS_ERR_CFG_INVALID_SUBKEY,                    TEXT("WFS_ERR_CFG_INVALID_SUBKEY"),
	WFS_ERR_CFG_INVALID_VALUE,                     TEXT("WFS_ERR_CFG_INVALID_VALUE"),
	WFS_ERR_CFG_KEY_NOT_EMPTY,                     TEXT("WFS_ERR_CFG_KEY_NOT_EMPTY"),
	WFS_ERR_CFG_NAME_TOO_LONG,                     TEXT("WFS_ERR_CFG_NAME_TOO_LONG"),
	WFS_ERR_CFG_NO_MORE_ITEMS,                     TEXT("WFS_ERR_CFG_NO_MORE_ITEMS"),
	WFS_ERR_CFG_VALUE_TOO_LONG,                    TEXT("WFS_ERR_CFG_VALUE_TOO_LONG"),
	WFS_ERR_DEV_NOT_READY,                         TEXT("WFS_ERR_DEV_NOT_READY"),
	WFS_ERR_HARDWARE_ERROR,                        TEXT("WFS_ERR_HARDWARE_ERROR"),
	WFS_ERR_INTERNAL_ERROR,                        TEXT("WFS_ERR_INTERNAL_ERROR"),
	WFS_ERR_INVALID_ADDRESS,                       TEXT("WFS_ERR_INVALID_ADDRESS"),
	WFS_ERR_INVALID_APP_HANDLE,                    TEXT("WFS_ERR_INVALID_APP_HANDLE"),
	WFS_ERR_INVALID_BUFFER,                        TEXT("WFS_ERR_INVALID_BUFFER"),
	WFS_ERR_INVALID_CATEGORY,                      TEXT("WFS_ERR_INVALID_CATEGORY"),
	WFS_ERR_INVALID_COMMAND,                       TEXT("WFS_ERR_INVALID_COMMAND"),
	WFS_ERR_INVALID_EVENT_CLASS,                   TEXT("WFS_ERR_INVALID_EVENT_CLASS"),
	WFS_ERR_INVALID_HSERVICE,                      TEXT("WFS_ERR_INVALID_HSERVICE"),
	WFS_ERR_INVALID_HPROVIDER,                     TEXT("WFS_ERR_INVALID_HPROVIDER"),
	WFS_ERR_INVALID_HWND,                          TEXT("WFS_ERR_INVALID_HWND"),
	WFS_ERR_INVALID_HWNDREG,                       TEXT("WFS_ERR_INVALID_HWNDREG"),
	WFS_ERR_INVALID_POINTER,                       TEXT("WFS_ERR_INVALID_POINTER"),
	WFS_ERR_INVALID_REQ_ID,                        TEXT("WFS_ERR_INVALID_REQ_ID"),
	WFS_ERR_INVALID_RESULT,                        TEXT("WFS_ERR_INVALID_RESULT"),
	WFS_ERR_INVALID_SERVPROV,                      TEXT("WFS_ERR_INVALID_SERVPROV"),
	WFS_ERR_INVALID_TIMER,                         TEXT("WFS_ERR_INVALID_TIMER"),
	WFS_ERR_INVALID_TRACELEVEL,                    TEXT("WFS_ERR_INVALID_TRACELEVEL"),
	WFS_ERR_LOCKED,                                TEXT("WFS_ERR_LOCKED"),
	WFS_ERR_NO_BLOCKING_CALL,                      TEXT("WFS_ERR_NO_BLOCKING_CALL"),
	WFS_ERR_NO_SERVPROV,                           TEXT("WFS_ERR_NO_SERVPROV"),
	WFS_ERR_NO_SUCH_THREAD,                        TEXT("WFS_ERR_NO_SUCH_THREAD"),
	WFS_ERR_NO_TIMER,                              TEXT("WFS_ERR_NO_TIMER"),
	WFS_ERR_NOT_LOCKED,                            TEXT("WFS_ERR_NOT_LOCKED"),
	WFS_ERR_NOT_OK_TO_UNLOAD,                      TEXT("WFS_ERR_NOT_OK_TO_UNLOAD"),
	WFS_ERR_NOT_STARTED,                           TEXT("WFS_ERR_NOT_STARTED"),
	WFS_ERR_NOT_REGISTERED,                        TEXT("WFS_ERR_NOT_REGISTERED"),
	WFS_ERR_OP_IN_PROGRESS,                        TEXT("WFS_ERR_OP_IN_PROGRESS"),
	WFS_ERR_OUT_OF_MEMORY,                         TEXT("WFS_ERR_OUT_OF_MEMORY"),
	WFS_ERR_SERVICE_NOT_FOUND,                     TEXT("WFS_ERR_SERVICE_NOT_FOUND"),
	WFS_ERR_SPI_VER_TOO_HIGH,                      TEXT("WFS_ERR_SPI_VER_TOO_HIGH"),
	WFS_ERR_SPI_VER_TOO_LOW,                       TEXT("WFS_ERR_SPI_VER_TOO_LOW"),
	WFS_ERR_SRVC_VER_TOO_HIGH,                     TEXT("WFS_ERR_SRVC_VER_TOO_HIGH"),
	WFS_ERR_SRVC_VER_TOO_LOW,                      TEXT("WFS_ERR_SRVC_VER_TOO_LOW"),
	WFS_ERR_TIMEOUT,                               TEXT("WFS_ERR_TIMEOUT"),
	WFS_ERR_UNSUPP_CATEGORY,                       TEXT("WFS_ERR_UNSUPP_CATEGORY"),
	WFS_ERR_UNSUPP_COMMAND,                        TEXT("WFS_ERR_UNSUPP_COMMAND"),
	WFS_ERR_VERSION_ERROR_IN_SRVC,                 TEXT("WFS_ERR_VERSION_ERROR_IN_SRVC"),
	WFS_ERR_INVALID_DATA,                          TEXT("WFS_ERR_INVALID_DATA"),
	WFS_ERR_SOFTWARE_ERROR,                        TEXT("WFS_ERR_SOFTWARE_ERROR"),
	WFS_ERR_CONNECTION_LOST,                       TEXT("WFS_ERR_CONNECTION_LOST"),
	WFS_ERR_USER_ERROR,                            TEXT("WFS_ERR_USER_ERROR"),
	WFS_ERR_UNSUPP_DATA,                           TEXT("WFS_ERR_UNSUPP_DATA"),
	WFS_ERR_FRAUD_ATTEMPT,                         TEXT("WFS_ERR_FRAUD_ATTEMPT"),
	WFS_ERR_SEQUENCE_ERROR,                        TEXT("WFS_ERR_SEQUENCE_ERROR"),	
	WFS_ERR_IDC_MEDIAJAM,                          TEXT("WFS_ERR_IDC_MEDIAJAM"),
	WFS_ERR_IDC_NOMEDIA,                           TEXT("WFS_ERR_IDC_NOMEDIA"),
	WFS_ERR_IDC_MEDIARETAINED,                     TEXT("WFS_ERR_IDC_MEDIARETAINED"),
	WFS_ERR_IDC_RETAINBINFULL,                     TEXT("WFS_ERR_IDC_RETAINBINFULL"),
	WFS_ERR_IDC_INVALIDDATA,                       TEXT("WFS_ERR_IDC_INVALIDDATA"),
	WFS_ERR_IDC_INVALIDMEDIA,                      TEXT("WFS_ERR_IDC_INVALIDMEDIA"),
	WFS_ERR_IDC_FORMNOTFOUND,                      TEXT("WFS_ERR_IDC_FORMNOTFOUND"),
	WFS_ERR_IDC_FORMINVALID,                       TEXT("WFS_ERR_IDC_FORMINVALID"),
	WFS_ERR_IDC_DATASYNTAX,                        TEXT("WFS_ERR_IDC_DATASYNTAX"),
	WFS_ERR_IDC_SHUTTERFAIL,                       TEXT("WFS_ERR_IDC_SHUTTERFAIL"),
	WFS_ERR_IDC_SECURITYFAIL,                      TEXT("WFS_ERR_IDC_SECURITYFAIL"),
	WFS_ERR_IDC_PROTOCOLNOTSUPP,                   TEXT("WFS_ERR_IDC_PROTOCOLNOTSUPP"),
	WFS_ERR_IDC_ATRNOTOBTAINED,                    TEXT("WFS_ERR_IDC_ATRNOTOBTAINED"),
	WFS_ERR_IDC_INVALIDKEY,                        TEXT("WFS_ERR_IDC_INVALIDKEY"),
	WFS_ERR_IDC_WRITE_METHOD,                      TEXT("WFS_ERR_IDC_WRITE_METHOD"),
	WFS_ERR_IDC_CHIPPOWERNOTSUPP,                  TEXT("WFS_ERR_IDC_CHIPPOWERNOTSUPP"),
	WFS_ERR_IDC_CARDTOOSHORT,                      TEXT("WFS_ERR_IDC_CARDTOOSHORT"),
	WFS_ERR_IDC_CARDTOOLONG,                       TEXT("WFS_ERR_IDC_CARDTOOLONG"),
	WFS_ERR_IDC_INVALID_PORT,                      TEXT("WFS_ERR_IDC_INVALID_PORT"),
	WFS_ERR_IDC_POWERSAVETOOSHORT,                 TEXT("WFS_ERR_IDC_POWERSAVETOOSHORT"),
	WFS_ERR_IDC_POWERSAVEMEDIAPRESENT,             TEXT("WFS_ERR_IDC_POWERSAVEMEDIAPRESENT"),
	WFS_ERR_IDC_CARDPRESENT,                       TEXT("WFS_ERR_IDC_CARDPRESENT"),
	WFS_ERR_IDC_POSITIONINVALID,                   TEXT("WFS_ERR_IDC_POSITIONINVALID")
};

void HMGetErrorText(const int hr, TCHAR *szOut)
{
	int i = 0;
	for ( i = 0; i < HMXFSErrCode_Num; ++i )
	{
		if ( HMXFSErrCode_Array[i].nValue == hr ) break;
	}

	//如果没有匹配项
	if ( i == HMXFSErrCode_Num )
	{
		_stprintf (szOut, TEXT(""));
	}
	else
	{
		_stprintf (szOut, TEXT("%s"), HMXFSErrCode_Array[i].szText);
	}
}