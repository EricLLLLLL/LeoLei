#ifndef DSTO_GLOBAL_H
#define DSTO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DSTO_LIBRARY)
#  define DSTOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DSTOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DSTO_GLOBAL_H
