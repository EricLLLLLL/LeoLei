#ifndef EASYTRACE_H
#define EASYTRACE_H

#include <QObject>
#include <QString>
#include <QMutex>
#include "win_type_def.h"
#include "easyloginterface.h"

class EasyTrace
{

public:
    EasyTrace();
    ~EasyTrace();

    void WriteLog(int nLevel, const char* szFormat, ...);

private:
    void PostEventLog(const QString&);
    QString __GetModuleFileName(void* func);
    QMutex m_mutex;
};

extern EasyTrace g_EasyTrace;


//四个等级的日志记录格式。
#define LOGINFO(fmt, ...)  g_EasyTrace.WriteLog(1, fmt, ##__VA_ARGS__);
#define LOGDEBUG(fmt, ...) g_EasyTrace.WriteLog(2, fmt, ##__VA_ARGS__);
#define LOGERROR(fmt, ...) g_EasyTrace.WriteLog(3, fmt, ##__VA_ARGS__);
#define LOGFATAL(fmt, ...) g_EasyTrace.WriteLog(4, fmt, ##__VA_ARGS__);

#endif // EASYTRACE_H
