#include "easytrace.h"
#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include "QDebug"
#include "QDir"
#include <QLibrary>

typedef long(*fnWriteLog)( LPVOID lpRecode );
fnWriteLog write = NULL;

EasyTrace g_EasyTrace;

// 防止加载失败用函数替换。
long EmptyFunction( LPVOID lpRecode )
{
    return -1;
}

int _vscprintf (const char * format, va_list pargs)
{
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}

EasyTrace::EasyTrace()
{

}

EasyTrace::~EasyTrace()
{
    DllEasyLog::UnLoadLibrary();
}


void EasyTrace::WriteLog(int nLevel, const char *szFormat, ...)
{
    m_mutex.lock();

    LPRecode lpRecodeMessage = new LogRecode();
    int len = 0;
    va_list  args;
    va_start(args, szFormat);
    len = _vscprintf(szFormat, args) + 1;
    lpRecodeMessage->nlevel = nLevel;
    lpRecodeMessage->buffer = new char[len * sizeof(char)];
    memset(lpRecodeMessage->buffer,0x00, sizeof lpRecodeMessage->buffer );
    //vsprintf_s(lpRecodeMessage->buffer, len, szFormat, args);
    vsprintf(lpRecodeMessage->buffer, szFormat, args);
    va_end(args);
    //::GetModuleFileNameA((HMODULE)AfxGetStaticModuleState()->m_hCurrentInstanceHandle,
    //    lpRecodeMessage->sModulName,sizeof lpRecodeMessage->sModulName);
    QString strModuleFileName = __GetModuleFileName((void*)&EasyTrace::WriteLog); //注意此处只能这样用，且在该类中调用的方法不能再调用LOGERRORE等输出日志，只能调用qt系统日志进行输出（如：qDebug之类），防止死循环
    QString strFileName= QCoreApplication::applicationFilePath();
    QString strCurPath = QDir::currentPath();
    memcpy(lpRecodeMessage->sModulName, Q2S(strModuleFileName), strModuleFileName.length());
    DllEasyLog::WriteLogFunc(lpRecodeMessage );

    delete[] lpRecodeMessage->buffer;
    lpRecodeMessage->buffer = NULL;
    delete lpRecodeMessage;
    lpRecodeMessage = NULL;

    m_mutex.unlock();
}

void EasyTrace::PostEventLog(const QString &str)
{
    QString strModuleFileName = __GetModuleFileName((void*)&EasyTrace::WriteLog);
    qCritical()<<str << " file:"<<strModuleFileName;
}

QString EasyTrace::__GetModuleFileName(void *func)
{
    QString strPath = "";
    Dl_info info;
    int rc;
    rc = dladdr(func, &info);
    if (!rc)  {
         qCritical("Problem retrieving program information for %x:  %s\n", func, dlerror());
         return "";
    }

    strPath = S2Q(info.dli_fname);
    return strPath;
}
