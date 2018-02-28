#ifndef MJNL_GLOBAL_H
#define MJNL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MJNL_LIBRARY)
#  define MJNLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MJNLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MJNL_GLOBAL_H
