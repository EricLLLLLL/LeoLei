#ifndef DRCV_GLOBAL_H
#define DRCV_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DRCV_LIBRARY)
#  define DRCVSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DRCVSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DRCV_GLOBAL_H
