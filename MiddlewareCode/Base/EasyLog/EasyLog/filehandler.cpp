#include "filehandler.h"
#include "easylog_global.h"
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include "common_def.h"

CFileHandler::CFileHandler(void)
{
    m_strLogDir = QCoreApplication::applicationDirPath() +DATA_DIR + "/MiddleTrace";
}


CFileHandler::~CFileHandler(void)
{

}

void CFileHandler::DealStart()
{
     //HANDLE m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)HandleFile,NULL,0,NULL);
     //CloseHandle(m_hThread);
}

DWORD CFileHandler::HandleFile(LPVOID lpParam)
{
    return 0;
}

bool CFileHandler::DeleteExpireLog(QString strLogPath)
{
    QString strFilePath = strLogPath;
    QString strExpireTime = QDateTime::currentDateTime().addDays(-30).toString("yyyyMMdd");
    QString strCurrentFileName;
    QString strTmp;

    QDir dir(strFilePath);
    if (!dir.exists())
    {
        return false;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    for (int i = 0; i < fileList.count(); ++i)
    {
        QFileInfo fileInfo = fileList.at(i);
        strCurrentFileName = fileInfo.fileName();
        if (strCurrentFileName.endsWith(EXTENSION))
        {
            strCurrentFileName = strCurrentFileName.left(8);
            if (strCurrentFileName <= strExpireTime)
            {
               // strCurrentFileName += EXTENSION;
               // strTmp = strFilePath + "/" + strCurrentFileName;
                QString strCurFile = fileInfo.absoluteFilePath();
                QFile::remove(strCurFile);
            }
        }
    }

    return true;
}

bool CFileHandler::DeleteAllExpireLogs()
{
    QString strDir = m_strLogDir;
    QString strCurrentFileName;

    QDir dir(strDir);
    if (!dir.exists())
    {
        return false;
    }
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QFileInfoList fileList = dir.entryInfoList();
    for (int i = 0; i < fileList.count(); ++i)
    {
        QFileInfo fileInfo = fileList.at(i);
        if (fileInfo.isDir())
        {
            strCurrentFileName = fileInfo.absoluteFilePath();
            DeleteExpireLog(strCurrentFileName);
        }
    }

    return true;
}
