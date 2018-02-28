#ifndef ICDPC_GLOBAL_H
#define ICDPC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ICDPC_LIBRARY)
#  define ICDPCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ICDPCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ICDPC_GLOBAL_H
