#ifndef TRACETESTDLL1_GLOBAL_H
#define TRACETESTDLL1_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TRACETESTDLL1_LIBRARY)
#  define TRACETESTDLL1SHARED_EXPORT Q_DECL_EXPORT
#else
#  define TRACETESTDLL1SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TRACETESTDLL1_GLOBAL_H
