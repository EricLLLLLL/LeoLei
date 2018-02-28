#ifndef EASYLOG_H
#define EASYLOG_H

#include "easylog_global.h"
#include "preprocessor.h"
#include "win_type_def.h"
#include "filehandler.h"
#include <QMutex>
#include <QObject>
#include "easyloginterface.h"

class CPreprocessor;
class CFileHandler;

class EASYLOGSHARED_EXPORT EasyLog : public IEasyLogInterface
{
public:
    EasyLog();
    ~EasyLog();

    static EasyLog* GetInstance();

    static void DestroyInstance();

    virtual long WriteLog(LPVOID lpRecode);

public:
    CPreprocessor m_preprocessor;
    QMutex m_mutex;
    CFileHandler m_filehandler;

    static EasyLog* m_pInstance;
};

extern "C"{
    EASYLOGSHARED_EXPORT EasyLog* CreateInstance(void);
    EASYLOGSHARED_EXPORT void DestroyInstance();
    EASYLOGSHARED_EXPORT long WriteLog(LPVOID lpRecode);
}

#endif // EASYLOG_H
