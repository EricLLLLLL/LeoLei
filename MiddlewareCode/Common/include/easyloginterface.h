#ifndef EASYLOGINTERFACE_H
#define EASYLOGINTERFACE_H
#include "win_type_def.h"
#include "QLibrary"
#include "QDebug"
#include "QDir"
#include "common_def.h"

typedef struct LogRecode{
    int nlevel;
    char sModulName[_MAX_FNAME];
    char *buffer;
}Recode, *LPRecode;

struct IEasyLogInterface
{
    virtual long WriteLog(LPVOID lpRecode) = 0;
};

namespace DllEasyLog {

typedef IEasyLogInterface* (*GetInterface)();
static QLibrary* pEasyLogLib = NULL;

typedef void (*DestroyInstance)();

typedef long(*fnWriteLog)( LPVOID lpRecode );

static bool LoadLibrary()
{
    if (pEasyLogLib == NULL)
    {
        QString strCurPath = QCoreApplication::applicationDirPath() + OUT_LIB_DIR;
        QString strLibPath = strCurPath + "libEasyLog.so";
        pEasyLogLib = new QLibrary(strLibPath);
        if(pEasyLogLib == NULL)
        {
            qCritical()<<"create QLibrary failed. strLibPath="<<strLibPath;
            return false;
        }

        if(!pEasyLogLib->load())
        {
            qCritical()<<"load module failed.  strLibPath="<<strLibPath<<"   error:"<<pEasyLogLib->errorString();
            return false;
        }
        return true;
    }
    return true;
}

static void UnLoadLibrary()
{
    if (pEasyLogLib)
    {
        DestroyInstance pFunc = (DestroyInstance)pEasyLogLib->resolve("DestroyInstance");
        if(pFunc)
        {
            pFunc();
        }
        pEasyLogLib->unload();
        SAFE_DELETE(pEasyLogLib);
    }
}

static IEasyLogInterface* Get_EasyLog_Interface()
{
    bool bLoad = LoadLibrary();
    if(!bLoad)
    {
        return NULL;
    }

    GetInterface pFunc = 0;
    pFunc = (GetInterface)pEasyLogLib->resolve("CreateInstance");
    if (pFunc)
    {
        IEasyLogInterface* pInterface = pFunc();
        return pInterface;
    }
    return NULL;
}

static long WriteLogFunc(LPVOID lpRecode)
{
    bool bLoad = LoadLibrary();
    if(!bLoad)
    {
        return -1;
    }

    fnWriteLog pFunc = 0;
    pFunc = (fnWriteLog)pEasyLogLib->resolve("WriteLog");
    if (pFunc)
    {
        return pFunc(lpRecode);
    }

    return -1;
}

}




#endif // EASYLOGINTERFACE_H
