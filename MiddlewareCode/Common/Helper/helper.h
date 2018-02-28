#ifndef HELPER_H
#define HELPER_H
#include <QString>
#include "win_type_def.h"

class CHelper
{

public:
    CHelper();

    //取当前模块路径名
    //func为当前调用的函数所在地址,，该函数要为该模块的成员函数
    //注意，如果是可执行程序使用该方法需要在pro文件的DEFINES配置中加上QT_APP
    QString GetModuleFileName(void* func);

    //取当前模块所在目录路径
    //func为当前调用的函数所在地址,，该函数要为该模块的成员函数
    //注意，如果是可执行程序使用该方法需要在pro文件的DEFINES配置中加上QT_APP
    QString GetModuelDirPath(void* func);

    QString GetPrivateProfileString(const QString &strAppName, const QString &strKeyName, const QString &strDefault, const QString &strCfgPath);

    bool GetPrivateProfileString(LPCSTR lpAppName, LPCSTR lpKeyName,LPCSTR lpDefault, LPSTR lpOutDest,DWORD nSize, LPCSTR lpCfgPath);

    int GetPrivateProfileInt(const QString &strAppName, const QString &strKeyName, int iDefault, const QString &strCfgPath);

    int GetPrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName,int iDefault, LPCSTR lpCfgPath);

    //在配置文件中写入值，如输入的文件路径不存在，则会自动创建文件包括目录，注意该方法会将原存在的配置文件中的注释清空
    bool WritePrivateProfileString(const QString &strAppName, const QString &strKeyName, const QString &strValue, const QString &strCfgPath);

    //从文件路径中截取文件夹路径，如（./test/test.ini截取后为./test/）
    QString ExtractDirPath(const QString &strFilePath);

    //从文件路径中截取文件名，如（./test/test.ini截取后为test.ini）
    QString ExtractFileName(const QString &strFilePath);

    //从文件路径中截取文件名称，不带扩展名，如（./test/test.ini截取后为test）
    QString ExtractFileNameNoExt(const QString &strFilePath);

    //创建目录路径
    bool CreateDirPath(const QString &strDirPath);

    //同CString::TrimLeft的用法
    void TrimLeft(QString& strSrc,const QString& strTarget);
};

extern CHelper g_Helper;

#endif // HELPER_H
