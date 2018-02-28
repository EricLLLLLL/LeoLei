#ifndef MRPT_GLOBAL_H
#define MRPT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MRPT_LIBRARY)
#  define MRPTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MRPTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MRPT_GLOBAL_H
