#include <QCoreApplication>
#include <QLibrary>
#include <common_def.h>
#include "Helper/helper.h"
#include "easytrace.h"

typedef long (*fnUserLog)();
fnUserLog userLog = NULL;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //EasyTrace::GetInstance();

    LOGDEBUG("TestDebug");
    LOGERROR("TestError");

    QString strModulePath = g_Helper.GetModuelDirPath(NULL);
    QString strLibPath = strModulePath + OUT_LIB_DIR +"libTraceTestDll.so";
    QLibrary * pLib = new QLibrary(strLibPath);
    if(pLib && pLib->load())
    {
        userLog = (fnUserLog)pLib->resolve("userLog1");
        if(userLog != NULL)
        {
            long iret = userLog();
        }
    }
    else
    {
        qDebug()<<"load library failed.  err:"<<pLib->errorString();
    }

    int a1 = 0;

    QString strLibPath1 = strModulePath + OUT_LIB_DIR +"libTraceTestDll1.so";
    QLibrary* pLib1 = new QLibrary(strLibPath1);
    if(pLib1 && pLib1->load())
    {
        userLog = (fnUserLog)pLib1->resolve("userLog2");
        if(userLog != NULL)
        {
            long iret = userLog();
        }
    }
    else
    {
        qDebug()<<"load library failed.  err:"<<pLib->errorString();
    }


    pLib->unload();
    pLib1->unload();
    SAFE_DELETE(pLib);
    SAFE_DELETE(pLib1);

    int iResult = a.exec();
    return iResult;
}
