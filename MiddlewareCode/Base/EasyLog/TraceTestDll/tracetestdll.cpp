#include "tracetestdll.h"
#include "QEventLoop"
#include "QTimer"

TraceTestDll* TraceTestDll::m_pInstance = NULL;

TraceTestDll::TraceTestDll()
{
    int a = 0;
}

TraceTestDll *TraceTestDll::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new TraceTestDll();
    }
    return m_pInstance;
}

long userLog1()
{
    while(1){
        LOGINFO("%s","This is the CUseDll1App trace which was written by easylog.");
        LOGDEBUG("%s","This is the CUseDll1App trace which was written by easylog.");
        LOGERROR("%s","This is the CUseDll1App trace which was written by easylog.");
        LOGINFO("%s","This is the CUseDll1App trace which was written by easylog.");
        LOGINFO("%s","This is the CUseDll1App trace which was written by easylog.");
        QEventLoop eventloop;
        QTimer::singleShot(100, &eventloop, SLOT(quit()));
        eventloop.exec();
        break;
       }
    return 1;
}
