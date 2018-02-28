#include "alarm.h"
#include "easylogging++.h"
#include "Lock.h"
#include <QCoreApplication>
#include <QDebug>
#include "common_def.h"
#include "basehelper.h"

Alarm::Alarm()
{
}
Alarm theApp;
QMutex g_cs;
QString strErrInfo;
QString g_strCurrentPath;   
BOOL Alarm::InitInstance()
{
	return TRUE;
}
inline void GetLogFilePath(char* szLogPath)
{
    QString strLogDirPath = QCoreApplication::applicationDirPath() + DATA_DIR + "/Alarm/";
    strcpy(szLogPath,Q2S(strLogDirPath));
    if ('/'!=szLogPath[strlen(szLogPath)-1])
	{
        strcat(szLogPath,"/");
	}
	strcat(szLogPath,ALARM_LOG_NAME);
}
void OutPutToLogFile(char* buf,int len)
{
	FILE* fFile;
	static BOOL s_bInit = FALSE;
	static char szLogPath[MAX_PATH]={0};
    QDateTime ctNow = QDateTime::currentDateTime();
	if (!s_bInit)
	{
        QString strLogPath = QCoreApplication::applicationDirPath() + DATA_DIR + "/Alarm/";
        g_BaseHelper.CreateDirPath(strLogPath);
        GetLogFilePath(szLogPath);
		s_bInit = TRUE;
	}
	try
	{
		if(fFile = fopen(szLogPath, "a+" ))
		{
            QString strNow = ctNow.toString("yyyy/MM/dd HH:mm:ss ");
            fwrite(Q2S(strNow),strNow.length(),1,fFile);
			fwrite(buf,len,1,fFile);
			fwrite("\n",1,1,fFile);
			fclose(fFile);
		}
	}
	catch (...)
	{
	}
}
extern "C" int RenameAlarmFile(void)
{
	char szLogPath[MAX_PATH]={0};
	GetLogFilePath(szLogPath);
    QDir dir(szLogPath);
    if(!dir.exists())
		return -1;
    QString strNewName=szLogPath;
    strNewName.replace(".",QDateTime::currentDateTime().toString("yyyyMMddHHmmss")+".");//如为"Alarm.txt",则改名为类似 "Alarm20161018091033.txt"
    return rename(szLogPath, Q2S(strNewName));
}


extern "C" long Alarm(const char* pszErrCode/*,HMODULE hcc = (HMODULE)AfxGetStaticModuleState()->m_hCurrentInstanceHandle*/)
{
	CLock g(&g_cs);
	if (0 == strcmp(pszErrCode,"00000000"))
	{
		strErrInfo = "";
	}
	else
	{
        QString strModulePath = g_BaseHelper.GetModuleFileName((void*)&Alarm);
        QString strDllName = g_BaseHelper.ExtractFileNameNoExt(strModulePath);
        strDllName = strDllName.toUpper();

        QString strAllModuleName = "MIDC读卡器|MCIM存款|MCDM取款|MFPI指纹仪|MIDR二代证|MPIN密码键盘|MRPT凭条打印机|MSIU传感器|MSCR刷卡器|MSYS系统控制|MTCP通信|MTSL交易统计|MJNL流水日志|MDAT数据|NONE未知模块|";
        QStringList listAllModuleName = strAllModuleName.split("|");
        QString strModuleName = strDllName;
        for(int iIndex = 0; iIndex < listAllModuleName.count();iIndex++)
        {
           QString strCurModuleName = listAllModuleName.at(iIndex);
           if(strCurModuleName.contains(strDllName))
           {
                strModuleName = strCurModuleName;
                break;
           }
        }

        QString strCfgPath = QCoreApplication::applicationDirPath() + CONST_ErrorCode_CFG;
        QString strDesription=g_BaseHelper.GetPrivateProfileString("ErrorCode",pszErrCode,"NONE", strCfgPath);
        DWORD dwGet = strDesription.length();
        char* szDesription = Q2S(strDesription);
        for (int i=7;i>0&&4==dwGet&&0==strcmp(szDesription,"NONE");i--)
		{
			char szBriefErrCode[9]="XXXXXXXX";
			memcpy(szBriefErrCode,pszErrCode,i);
            strDesription = g_BaseHelper.GetPrivateProfileString("ErrorCode",szBriefErrCode,"NONE",strCfgPath);
            dwGet = strDesription.length();
            szDesription = Q2S(strDesription);
        }
		char szErr[_MAX_FNAME*6]={0};
		//TestDll.exe|01000000|读卡器故障@
        sprintf(szErr,"%s|%s|%s", Q2S(strModuleName), pszErrCode, szDesription);
        OutPutToLogFile(szErr,strlen(szErr));
		strErrInfo = szErr;
	}
	return 0;
}

extern "C" int GetAlarm(char* pszOutput, const int nBuffLen=0)
{
	CLock g(&g_cs);
	if (0 > nBuffLen)
	{
		return -1;
	}
    int nLenOfstr = (strErrInfo.isEmpty())?0:strErrInfo.length()+1;
	if (NULL == pszOutput||0 == nBuffLen)
	{
		return nLenOfstr;
	}
	ZeroMemory(pszOutput, nBuffLen);
	int nLenOfCopy = min(nBuffLen-1,nLenOfstr);
	if (0 < nLenOfCopy)
	{
        memcpy(pszOutput,Q2S(strErrInfo),nLenOfCopy);
	}
	return nLenOfCopy;
}
