#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "win_type_def.h"
#include <QString>

class CFileHandler
{
public:
    CFileHandler(void);
    ~CFileHandler(void);
    void DealStart();
//	HANDLE m_hThread;
    static DWORD HandleFile(LPVOID lpParam);
    bool DeleteExpireLog(QString);
    bool DeleteAllExpireLogs();

private:
    QString m_strLogDir;
};

#endif // FILEHANDLER_H
