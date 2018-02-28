#ifndef MTSL_GLOBAL_H
#define MTSL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MTSL_LIBRARY)
#  define MTSLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MTSLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MTSL_GLOBAL_H
