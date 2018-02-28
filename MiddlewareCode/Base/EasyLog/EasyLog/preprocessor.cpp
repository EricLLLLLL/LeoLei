#include "preprocessor.h"
#include "easylog_global.h"
#include <QDateTime>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <common_def.h>
#include "basehelper.h"

CPreprocessor::CPreprocessor(void)
{
    memset(m_szDirPath, 0x00, sizeof(m_szDirPath));
    memset(m_szExpiration, 0x00, sizeof(m_szExpiration));
    memset(m_szPathConfig, 0x00, sizeof(m_szPathConfig));

    m_strLogDir = QCoreApplication::applicationDirPath() +DATA_DIR + "/MiddleTrace";
    CreateConfiguration();
}


CPreprocessor::~CPreprocessor(void)
{
}

void CPreprocessor::AssignPathFile(char* inInvokeModulePath, char* sModuleNamePath, char* sOutModuleName)
{
    GetConfiguration();

    QString strInInvlkeModulePath = inInvokeModulePath;
    QString strFile = g_BaseHelper.ExtractFileNameNoExt(strInInvlkeModulePath);
    QString strModuleName = g_BaseHelper.GetPrivateProfileString("LOG_NAME", strFile, "INEXISTENCE", m_szPathConfig);

    if (strModuleName == "INEXISTENCE")
    {
        g_BaseHelper.WritePrivateProfileString("LOG_NAME", strFile, strFile, m_szPathConfig);
        strModuleName = strFile;
    }

    QString strBuffer = QString("%1/%2/%3%4")
            .arg(m_szDirPath).arg(strModuleName)
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd")).arg(EXTENSION);
    if (strModuleName == strFile)
    {
        memcpy(sOutModuleName, Q2S(strModuleName), strModuleName.length());
    }
    else
    {
        memcpy(sOutModuleName, Q2S(strFile), strFile.length());
    }
    memcpy(sModuleNamePath, Q2S(strBuffer), strBuffer.length());
}




void CPreprocessor::GetConfiguration()
{
    QString strDirPath = g_BaseHelper.GetPrivateProfileString("CONF", "LOG_DIR", m_strLogDir, m_szPathConfig);
    QString strExpiration = g_BaseHelper.GetPrivateProfileString("CONF", "ExpirationDays", EXPIRATION, m_szPathConfig);
    memcpy(m_szDirPath, Q2S(strDirPath), strDirPath.length());
    memcpy(m_szExpiration, Q2S(strExpiration), strExpiration.length());
}

void CPreprocessor::CreateConfiguration()
{
    std::ofstream fpConfigurationfile;
    QString strDllPath = g_BaseHelper.GetModuleFileName((void*)&CPreprocessor::CreateConfiguration);
    QString strPathConfig = g_BaseHelper.ExtractDirPath(strDllPath);
    strPathConfig += CONFIGNAME;
    int iPathSize = sizeof(m_szPathConfig);
    strncpy(m_szPathConfig,Q2S(strPathConfig),std::min(strPathConfig.size(),iPathSize));

    QString strDirPath = g_BaseHelper.GetPrivateProfileString("CONF", "LOG_DIR", "0", strPathConfig);
    int iDirSize = sizeof(m_szDirPath);
    strncpy(m_szDirPath, Q2S(strDirPath), std::min(strDirPath.size(),iDirSize));

    if (strcmp(m_szDirPath, "0") == 0)
    {
        fpConfigurationfile.open(m_szPathConfig, std::ios::app);
        QString strComment(";The configuration file will be generated loading by anynoe else.\n\
                           ;You can replace the default value of each config.such as LOG_DIR,Expiration and LOG_NAME\n\n");
        fpConfigurationfile.write(Q2S(strComment), strComment.length());
        fpConfigurationfile.flush();
        fpConfigurationfile.close();

        g_BaseHelper.WritePrivateProfileString("CONF", "LOG_DIR", m_strLogDir, m_szPathConfig);
        g_BaseHelper.WritePrivateProfileString("CONF", "ExpirationDays", EXPIRATION, m_szPathConfig);

    }
}
