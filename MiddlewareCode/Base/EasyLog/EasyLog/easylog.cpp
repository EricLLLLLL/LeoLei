#include "easylog.h"
#include "easylogging++.h"
#include <stdarg.h>

_INITIALIZE_EASYLOGGINGPP

EasyLog* EasyLog::m_pInstance = NULL;

EasyLog::EasyLog()
{
    m_filehandler.DeleteAllExpireLogs();
}

EasyLog::~EasyLog()
{

}

EasyLog *EasyLog::GetInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new EasyLog();
    }

    return m_pInstance;
}

void EasyLog::DestroyInstance()
{
    SAFE_DELETE(m_pInstance);
}

long EasyLog::WriteLog(LPVOID lpRecode)
{
    m_mutex.lock();

    char szLogFilenname[_MAX_FNAME]={0};
    char szModulename[_MAX_FNAME]={0};
    LPRecode lpRecodeMessage = (LPRecode)lpRecode;

    m_preprocessor.AssignPathFile(lpRecodeMessage->sModulName,szLogFilenname,szModulename);
    easyloggingpp::Loggers::setFilename(szLogFilenname);
    switch( lpRecodeMessage->nlevel )
    {
    case 1:
        LOG(INFO)<<"【"<<szModulename<<":"<< pthread_self() <<"】"<<lpRecodeMessage->buffer;
        break;
    case 2:
        LOG(DEBUG)<<"【"<<szModulename<<":"<< pthread_self() <<"】"<<lpRecodeMessage->buffer;
        break;
    case 3:
        LOG(ERROR)<<"【"<<szModulename<<":"<< pthread_self() <<"】"<<lpRecodeMessage->buffer;
        break;
    case 4:
        LOG(FATAL)<<"【"<<szModulename<<":"<< pthread_self() <<"】"<<lpRecodeMessage->buffer;
        break;
    default:
        LOG(TRACE)<<"【"<<szModulename<<":"<< pthread_self() <<"】"<<lpRecodeMessage->buffer;
        break;
    }

    m_mutex.unlock();
    return 1;
}

EasyLog* CreateInstance()
{
    return EasyLog::GetInstance();
}

long WriteLog(LPVOID lpRecode)
{
    return EasyLog::GetInstance()->WriteLog(lpRecode);
}

void DestroyInstance()
{
    EasyLog::DestroyInstance();
}
