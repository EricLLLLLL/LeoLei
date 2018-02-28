#ifndef ALARM_GLOBAL_H
#define ALARM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ALARM_LIBRARY)
#  define ALARMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ALARMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ALARM_GLOBAL_H
