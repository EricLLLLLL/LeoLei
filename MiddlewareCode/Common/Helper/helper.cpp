#include "helper.h"
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <dlfcn.h>
#include <QDir>
#include <QCoreApplication>
#include "easytrace.h"

CHelper g_Helper;

CHelper::CHelper()
{

}

QString CHelper::GetModuleFileName(void* func)
{
    QString strPath = "";
    #ifndef QT_APP
        Dl_info info;
        int rc;
        rc = dladdr(func, &info);
        if (!rc)  {
             qCritical("Problem retrieving program information for %x:  %s\n", func, dlerror());
             return "";
        }

        strPath = S2Q(info.dli_fname);
    #else
        strPath = QCoreApplication::applicationFilePath();
    #endif

        return strPath;
}

QString CHelper::GetModuelDirPath(void *func)
{
    QString strModuleFileName = GetModuleFileName(func);
    QString strDirPath = ExtractDirPath(strModuleFileName);
    return strDirPath;
}

QString CHelper::GetPrivateProfileString(const QString &strAppName, const QString &strKeyName, const QString &strDefault, const QString &strCfgPath)
{
    if (!QFile::exists(strCfgPath))
    {
        LOGERROR("file not exist.  path=%s",Q2S(strCfgPath));
        return strDefault;
    }

    QSettings oInitSettings(strCfgPath,QSettings::IniFormat);
    oInitSettings.setIniCodec("UTF-8");
    QString strIniKey = "/" + strAppName +"/" +strKeyName;
    QString strValue = oInitSettings.value(strIniKey,strDefault).toString();
    return strValue;
}

bool CHelper::GetPrivateProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpOutDest, DWORD nSize, LPCSTR lpCfgPath)
{
    if(lpAppName == NULL || lpKeyName == NULL || lpDefault == NULL
            || lpOutDest == NULL || nSize == 0 || lpCfgPath == NULL)
    {
        return false;
    }

    QString strAppName = S2Q(lpAppName);
    QString strKeyName = S2Q(lpKeyName);
    QString strDefault = S2Q(lpDefault);
    QString strCfgPath = S2Q(lpCfgPath);
    QString strOut = GetPrivateProfileString(strAppName,strKeyName,strDefault,strCfgPath);
    strncpy(lpOutDest,Q2S(strOut),std::min(strOut.size(),(int)nSize));
    return true;
}

int CHelper::GetPrivateProfileInt(const QString &strAppName, const QString &strKeyName, int iDefault, const QString &strCfgPath)
{
    if (!QFile::exists(strCfgPath))
    {
        LOGERROR("file not exist.  path=%s", Q2S(strCfgPath));
        return iDefault;
    }

    QSettings oInitSettings(strCfgPath,QSettings::IniFormat);
    QString strIniKey = "/" + strAppName +"/" +strKeyName;
    int iValue = oInitSettings.value(strIniKey,iDefault).toInt();
    return iValue;
}

int CHelper::GetPrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName,int iDefault, LPCSTR lpCfgPath)
{
    if(lpAppName == NULL || lpKeyName == NULL || lpCfgPath == NULL)
    {
        return iDefault;
    }

    QString strAppName = S2Q(lpAppName);
    QString strKeyName = S2Q(lpKeyName);
    QString strCfgPath = S2Q(lpCfgPath);
    return GetPrivateProfileInt(strAppName,strKeyName,iDefault,strCfgPath);
}

bool CHelper::WritePrivateProfileString(const QString &strAppName, const QString &strKeyName, const QString &strValue, const QString &strCfgPath)
{
    if (!QFile::exists(strCfgPath))
    {
        QString strDir = ExtractDirPath(strCfgPath);
        QDir dir;
        if(!dir.exists(strDir))
        {
            dir.mkpath(strDir);
        }

        QFile file(strCfgPath);
        bool bOpen = file.open(QIODevice::ReadWrite);
        if(!bOpen)
        {
            QString strErr = file.errorString();
            LOGERROR("open file failed.  path=%s  err:%s", Q2S(strCfgPath), Q2S(strErr));
            return false;
        }
    }

    QSettings oInitSettings(strCfgPath,QSettings::IniFormat);
    QString strIniKey = "/" + strAppName +"/" +strKeyName;
    oInitSettings.setValue(strIniKey,strValue);
    return true;
}

QString CHelper::ExtractDirPath(const QString &strFilePath)
{
    QString strTemp = strFilePath;
    if (strTemp.contains("\\"))
    {
        strTemp = strTemp.replace("\\", "/");
    }

    int iPos1 = strTemp.lastIndexOf("/");
    return strTemp.mid(0,iPos1+1);
}

QString CHelper::ExtractFileName(const QString &strFilePath)
{
    QString strTemp = strFilePath;
    if (strTemp.contains("\\"))
    {
        strTemp = strTemp.replace("\\", "/");
    }

    int iPos1 = strTemp.lastIndexOf("/");
    return strTemp.mid(iPos1+1);
}

QString CHelper::ExtractFileNameNoExt(const QString &strFilePath)
{
    QString strTemp = strFilePath;
    if (strTemp.contains("\\"))
    {
        strTemp = strTemp.replace("\\", "/");
    }

    int iPos1 = strTemp.lastIndexOf("/");
    int iPos2 = strTemp.lastIndexOf('.');
    if(iPos2 < 0)
    {
        iPos2 = strTemp.size();
    }
    return strTemp.mid(iPos1+1,iPos2-iPos1-1);
}

bool CHelper::CreateDirPath(const QString &strDirPath)
{
    QDir dir;
    if(!dir.exists(strDirPath))
    {
        return dir.mkpath(strDirPath);
    }

    return true;
}

void CHelper::TrimLeft(QString &strSrc, const QString &strTarget)
{
    if(strSrc.isEmpty() || strTarget.isEmpty())
    {
        return;
    }

    int iIndex = 0;
    while(strSrc[iIndex] != 0 && (strTarget.indexOf(strSrc[iIndex]) != -1))
    {
        iIndex++;
    }

    strSrc = strSrc.mid(iIndex,strSrc.length());
}

