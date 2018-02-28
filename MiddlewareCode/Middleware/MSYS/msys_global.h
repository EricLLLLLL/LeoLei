#ifndef MSYS_GLOBAL_H
#define MSYS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MSYS_LIBRARY)
#  define MSYSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MSYSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MSYS_GLOBAL_H
