#ifndef MSIU_GLOBAL_H
#define MSIU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MSIU_LIBRARY)
#  define MSIUSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MSIUSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MSIU_GLOBAL_H
