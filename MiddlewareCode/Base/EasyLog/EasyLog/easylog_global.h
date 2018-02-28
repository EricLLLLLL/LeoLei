#ifndef EASYLOG_GLOBAL_H
#define EASYLOG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EASYLOG_LIBRARY)
#  define EASYLOGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define EASYLOGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // EASYLOG_GLOBAL_H
