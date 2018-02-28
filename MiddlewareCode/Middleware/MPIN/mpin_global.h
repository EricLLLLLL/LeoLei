#ifndef MPIN_GLOBAL_H
#define MPIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MPIN_LIBRARY)
#  define MPINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MPINSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MPIN_GLOBAL_H
