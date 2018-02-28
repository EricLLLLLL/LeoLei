#ifndef MIDR_GLOBAL_H
#define MIDR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MIDR_LIBRARY)
#  define MIDRSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MIDRSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MIDR_GLOBAL_H
